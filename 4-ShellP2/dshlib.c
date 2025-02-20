#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"



int dsh_cd(int argc, char *argv[]) {
    // If no argument is given, do nothing (per assignment requirement)
    if (argc == 1) {
        return 0;
    }

    // Attempt to change directory
    if (chdir(argv[1]) != 0) {
        perror("cd");  // Print error if chdir() fails
        return -1;
    }

    return 0;
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
            waitpid(pid, &status, 0);
        }

        // Free allocated memory
        for (int i = 0; i < arg_count; i++) free(args[i]);
    }

    return OK;
}


