#include "main.h"
#include "parse_cmd.h"
#include "command_implementation.h"
#include "sys_commands.h"
#include "io.h"
#include "history.h"
#include "misc.h"

// inp: semicolon separated inputs
// idea: char** commands
// split by ; and then store in array
// loop through array
// execute one by one
// so sed
// returns -2 if "quit" command is
// encountered
int separate_cmd(char* inp) {
    int quit = 0;
    unsigned long num_cmds = 0;

    char** cmd_arr = (char**) malloc(sizeof(char) * (strlen(inp) + 10));

    char* cmd = strtok(inp, ";");
    while (cmd != NULL) {
        cmd_arr[num_cmds] = (char*) malloc(sizeof(char) * (strlen(cmd) + 100));
        strcpy(cmd_arr[num_cmds++], cmd);

        cmd = strtok(NULL, ";");
    }

    for (int i = 0; i < num_cmds; i++) {
        if (handle_cmd(cmd_arr[i]) == -2)
            quit = -2;
        free(cmd_arr[i]);
    }
    free(cmd_arr);
    return quit;
}

// Returns -2 if "quit" command is encountered
int handle_cmd(char* inp_cmd) {
    char* inp = (char*) malloc(sizeof(char) * (strlen(inp_cmd)));
    strcpy(inp, inp_cmd);
    char* cmd = (char*) malloc(sizeof(char) * (strlen(inp)+10));
    char** cmd_args = (char**) malloc(sizeof(char*) * strlen(inp));
    int cmd_len = 0;

    // tokenize
    char* line = (char*) malloc(sizeof(char)*(strlen(inp)+10));
    strcpy(line, inp);

    char* chnk = strtok(line, " \t");                     // *************************8 CHANGE
    if (chnk == NULL) {
        fprintf(stderr, "Abruptly quitting tokenizing command\n");
        return -1;
    }
    strcpy(cmd, chnk);
    
    // should be alloted by caller
    // cmd_args = (char**) malloc(sizeof(char**) * strlen(raw_input)+1);
    chnk = strtok(NULL, " \t");
    while (chnk != NULL) {
        cmd_args[cmd_len] = (char*) malloc(sizeof(char) * (strlen(chnk) + strlen(homedir) + 100));  // TODO: change size
        strcpy(cmd_args[cmd_len], chnk);
        cmd_len++;

        chnk = strtok(NULL, " \t");
    }
    free(line);

    // for (int i = 0; i < cmd_len; i++) {
    //     printf("arg: %s || ", cmd_args[i]);
    // }
    // printf("\n");
    // printf("------ NOW HANDLING COMMAND-----------\n");

    char* hist = (char*) malloc(sizeof(char) * HIST_STR_SIZE);
    build_cmd(cmd, cmd_args, cmd_len, hist);
    insert_history(hist);
    free(hist);

    int quit = 0;
    if (!strcmp(cmd, "cd")) {
        cd_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "echo")) {
        echo_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "pwd")) {
        pwd_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "ls")) {
        ls_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "pinfo")) {
        pinfo_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "history")) {
        history_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "quit")) {
        printf("Attempting to quit\n");
        quit = -2;
    }
    else {
        if (cmd_len >= 1 && strcmp(cmd_args[cmd_len-1], "&") == 0) {
            bg_execution(cmd, cmd_args, cmd_len - 1);
        } else {
            fg_execution(cmd, cmd_args, cmd_len);
        }
    }

    for (int i = 0; i < cmd_len; i++) {
        free(cmd_args[i]);
    }

    /* freedom */
    free(cmd_args);
    free(cmd);
    free(inp);

    return quit;
}
