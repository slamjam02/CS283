#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{
    char *cmd_buff;
    int rc = 0;
    cmd_buff_t cmd;

    while(1){

        printf("%s", SH_PROMPT);

        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
            printf("\n");
            break;
        }

        //remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';

        // Trim leading spaces
        while (*cmd && isspace((unsigned char) *cmd)) {
            cmd++;
        }

        // Trim trailing spaces
        char *end = cmd + strlen(cmd) - 1;
        while (end > cmd && isspace((unsigned char) *end)) {
            *end = '\0';
            end--;
        }

        command_t *cur_cmd = &clist->commands[cmd_count];

        char *arg;
        char *arg_rest = cmd;
        int arg_count = 0;

        while ((arg = strtok_r(arg_rest, " ", &arg_rest)) != NULL) {
            // First string is the executable
            if (arg_count == 0) {
                if (strlen(arg) >= EXE_MAX) return ERR_CMD_OR_ARGS_TOO_BIG;

                strncpy(cur_cmd->exe, arg, EXE_MAX - 1);
                cur_cmd->exe[EXE_MAX - 1] = '\0';

            // Other strings are arguments
            } else {

                if (strlen(cur_cmd->args) + strlen(arg) + 1 >= ARG_MAX) {
                    return ERR_CMD_OR_ARGS_TOO_BIG;
                }

                // Add space after first argument if there are more than one
                if (arg_count > 1) {
                    strncat(cur_cmd->args, " ", ARG_MAX - strlen(cur_cmd->args) - 1);
                }

                // Concatenate argument to arg string
                strncat(cur_cmd->args, arg, ARG_MAX - strlen(cur_cmd->args) - 1);
            }

            arg_count++;
        }

        
 





        //IMPLEMENT THE REST OF THE REQUIREMENTS
    }

    // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"

    return OK;
}
