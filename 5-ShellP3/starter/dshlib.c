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
    if (argc == 1) {
        return 0;
    }

    if (argc > 2) {
        perror("cd");
        return -1;
    }

    if (chdir(argv[1]) != 0) {
        perror("cd");
        return -1;
    }

    return 0;
}

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist) {
    memset(clist, 0, sizeof(command_list_t));

    char *cmd;
    char *cmd_buff = strdup(cmd_line);
    char *rest = cmd_buff;
    int cmd_count = 0;

    while ((cmd = strtok_r(rest, PIPE_STRING, &rest)) != NULL) {
        if (cmd_count >= CMD_MAX) {
            free(cmd_buff);
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

        cmd_buff_t *cur_cmd = &clist->commands[cmd_count];
        char *arg_start = NULL;
        bool in_quotes = false;
        int arg_count = 0;

        char *ptr = cmd;
        while (*ptr) {
            if (*ptr == '"') {
                in_quotes = !in_quotes;
                if (!in_quotes) {
                    *ptr = '\0';
                } else {
                    arg_start = ptr + 1;
                }
            } else if (isspace((unsigned char) *ptr) && !in_quotes) {
                if (arg_start) {
                    *ptr = '\0';
                    if (arg_count >= CMD_ARGV_MAX - 1) {
                        free(cmd_buff);
                        return ERR_MEMORY;
                    }
                    cur_cmd->argv[arg_count++] = strdup(arg_start);
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
            cur_cmd->argv[arg_count++] = strdup(arg_start);
        }

        cur_cmd->argv[arg_count] = NULL;
        cur_cmd->argc = arg_count;
        cmd_count++;
    }

    clist->num = cmd_count;
    free(cmd_buff);
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
int exec_local_cmd_loop() {
    char cmd_buff[SH_CMD_MAX];
    command_list_t command_list;

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        int result = build_cmd_list(cmd_buff, &command_list);
        if (result == WARN_NO_CMDS) {
            printf("%s\n", CMD_WARN_NO_CMD);
            continue;
        } else if (result == ERR_TOO_MANY_COMMANDS) {
            printf("%s\n", CMD_ERR_PIPE_LIMIT);
            continue;
        }

        int pipes[CMD_MAX - 1][2];
        for (int i = 0; i < command_list.num - 1; i++) {
            if (pipe(pipes[i]) == -1) {
                perror("pipe failed");
                return ERR_MEMORY;
            }
        }

        for (int i = 0; i < command_list.num; i++) {
            cmd_buff_t *cmd = &command_list.commands[i];

            // Handle built-in commands
            if (strcmp(cmd->argv[0], "exit") == 0) {
                printf("Exiting shell...\n");
                return 0;
            } 
            if (strcmp(cmd->argv[0], "cd") == 0) {
                dsh_cd(cmd->argc, cmd->argv);
                continue;
            }

            pid_t pid = fork();
            if (pid == -1) {
                perror("fork failed");
                return ERR_MEMORY;
            }

            if (pid == 0) {
                // Set up pipes
                if (i > 0) {
                    dup2(pipes[i - 1][0], STDIN_FILENO);
                }
                if (i < command_list.num - 1) {
                    dup2(pipes[i][1], STDOUT_FILENO);
                }

                // Close unused pipes
                for (int j = 0; j < command_list.num - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                execvp(cmd->argv[0], cmd->argv);
                perror(CMD_ERR_EXECUTE);
                exit(1);
            }
        }

        // Close all pipes in parent
        for (int i = 0; i < command_list.num - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        // Wait for all child processes
        for (int i = 0; i < command_list.num; i++) {
            wait(NULL);
        }
    }

    return OK;
}