#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"




int build_cmd_list(char *cmd_line, command_list_t *clist) {
    memset(clist, 0, sizeof(command_list_t));

    char *cmd;
    char *rest = cmd_line;
    int cmd_count = 0;

    while ((cmd = strtok_r(rest, PIPE_STRING, &rest)) != NULL) {
        if (cmd_count >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

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

        if (strlen(cmd) == 0) { 
            continue;
        }

        command_t *cur_cmd = &clist->commands[cmd_count];

        char *arg;
        char *arg_rest = cmd;
        int arg_count = 0;

        // Split tokens and iterate
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

        cmd_count++;
    }

    clist->num = cmd_count;
    return (cmd_count == 0) ? WARN_NO_CMDS : OK;
}






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
    char *cmd_buff = malloc(SH_CMD_MAX);
    if (!cmd_buff) return ERR_MEMORY;

    cmd_buff_t cmd;

    while (1) {
        printf("%s", SH_PROMPT);

        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        // Remove trailing newline
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        // Trim leading spaces
        while (*cmd_buff && isspace((unsigned char)*cmd_buff)) {
            cmd_buff++;
        }

        // Trim trailing spaces
        char *end = cmd_buff + strlen(cmd_buff) - 1;
        while (end > cmd_buff && isspace((unsigned char)*end)) {
            *end = '\0';
            end--;
        }

        char *args[CMD_ARGV_MAX] = {NULL};  // Argument list
        int arg_count = 0;
        char *ptr = cmd_buff;
        bool in_quotes = false;
        char *arg_start = NULL;

        while (*ptr) {
            if (*ptr == '"') {
                in_quotes = !in_quotes;
                if (!in_quotes) {
                    *ptr = '\0';
                } else {
                    arg_start = ptr + 1;
                }
            } else if (isspace((unsigned char)*ptr) && !in_quotes) {
                if (arg_start) {
                    *ptr = '\0';

                    if (arg_count >= CMD_ARGV_MAX - 1) {
                        return ERR_MEMORY;                      
                    }

                    args[arg_count++] = strdup(arg_start);
                    arg_start = NULL;
                }
            } else {
                if (!arg_start) {
                    arg_start = ptr;
                }
            }
            ptr++;
        }
        if (arg_start) {
            args[arg_count++] = strdup(arg_start);
        }
        args[arg_count] = NULL;

        if (arg_count == 0) {
            printf("%s\n", CMD_WARN_NO_CMD);
            continue;
        }

        cmd.argc = arg_count;
        for (int i = 0; i < arg_count; i++) {
            cmd.argv[i] = args[i];
        }
        cmd.argv[arg_count] = NULL;

        // Built-in commands
        if (strcmp(args[0], "exit") == 0) {
            printf("Exiting shell...\n");
            for (int i = 0; i < arg_count; i++) free(args[i]);
            free(cmd_buff);
            exit(0);
        }
        if (strcmp(args[0], "cd") == 0) {
            dsh_cd(cmd.argc, cmd.argv);
            continue;
        }

        // External commands
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork failed");
            exit(1);
        } else if (pid == 0) {
            execvp(args[0], args);
            perror("execvp failed");
            exit(1);
        } else {
            int status;
            wait(&status);
        }


        // Free allocated memory
        for (int i = 0; i < arg_count; i++) free(args[i]);
    }

    return OK;
}
