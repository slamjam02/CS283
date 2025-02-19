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
        while (*cmd_buff && isspace((unsigned char) *cmd_buff)) {
            cmd_buff++;
        }

        // Trim trailing spaces
        char *end = cmd_buff + strlen(cmd_buff) - 1;
        while (end > cmd_buff && isspace((unsigned char) *end)) {
            *end = '\0';
            end--;
        }

        cmd_buff_t *cur_cmd = malloc(sizeof(cmd_buff_t));

        char *arg;
        char *arg_rest = cmd_buff;
        int arg_count = 0;

        while ((arg = strtok_r(arg_rest, " ", &arg_rest)) != NULL) {
            // First string is the executable
            if (arg_count == 0) {
                if (strlen(arg) >= EXE_MAX) return ERR_CMD_OR_ARGS_TOO_BIG;

                strncpy(cur_cmd->_cmd_buffer, arg, EXE_MAX - 1);
                cur_cmd->_cmd_buffer[EXE_MAX - 1] = '\0';

            // Other strings are arguments
            } else {

                // if (strlen(cur_cmd->args) + strlen(arg) + 1 >= ARG_MAX) {
                //     return ERR_CMD_OR_ARGS_TOO_BIG;
                // }
                if(arg_count >= CMD_ARGV_MAX){
                    
                }

                // Add space after first argument if there are more than one

                // Concatenate argument to arg string
                strncpy(cur_cmd->argv[arg_count - 1], arg, ARG_MAX);
            }

            arg_count++;
        }

        cur_cmd->argc = arg_count;


        
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork failed");
            exit(1);
        } else if (pid == 0) {  // Child process
            execv(cur_cmd->_cmd_buffer, cur_cmd->argv); // Replace with new process
            perror("execlp failed"); // Only runs if execlp fails
            exit(1);
        } else {  // Parent process
            int status;
            waitpid(pid, &status, 0); // Wait for child to finish
            printf("Child process finished.\n");
        }

 

        free(cur_cmd);
    }

    // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"

    return OK;
}
