#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

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