#include "main.h"
#include "parse_cmd.h"
#include "pipe.h"
#include "command_implementation.h"
#include "sys_commands.h"
#include "io.h"
#include "history.h"
#include "redirection.h"
#include "env.h"
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

    char** cmd_arr = (char**) malloc(sizeof(char*) * (strlen(inp) + 10));

    char* cmd = strtok(inp, ";");
    while (cmd != NULL) {
        cmd_arr[num_cmds] = (char*) malloc(sizeof(char) * (strlen(cmd) + 100));
        strcpy(cmd_arr[num_cmds++], cmd);

        cmd = strtok(NULL, ";");
    }

    int ret;
    for (int i = 0; i < num_cmds; i++) {
        ret = mother_pipe(cmd_arr[i]);
    }

    for (int i = 0; i < num_cmds; i++)
        free(cmd_arr[i]);
    free(cmd_arr);

    return ret;
}

// Returns -2 if "quit" command is encountered
int handle_cmd(char* inp_cmd) {
    char* inp = (char*) malloc(sizeof(char) * (strlen(inp_cmd)));
    strcpy(inp, inp_cmd);
    char* cmd = (char*) malloc(sizeof(char) * (strlen(inp)+10));
    char** cmd_args = (char**) malloc(sizeof(char*) * strlen(inp));
    int cmd_len = 0;
    is_append = -1;
    is_out_redirect = 0;
    is_in_redirect = 0;

    curr_fg_pid = -1;

    // tokenize
    char* line = (char*) malloc(sizeof(char)*(strlen(inp)+10));
    strcpy(line, inp);

    char* chnk = strtok(line, " \t");                     // *************************8 CHANGE
    if (chnk == NULL) {
        return 0;
    }
    strcpy(cmd, chnk);

    strcpy(curr_fg_name, cmd);  /* foreground  */
    
    // should be alloted by caller
    // cmd_args = (char**) malloc(sizeof(char**) * strlen(raw_input)+1);
    chnk = strtok(NULL, " \t");
    while (chnk != NULL) {
        if (strcmp(chnk, ">") == 0 || strcmp(chnk, ">>") == 0) {
            is_append = 0; is_out_redirect = 1;
            if (strcmp(chnk, ">>") == 0) { is_append = 1; }

            chnk = strtok(NULL, " \t");

            if (chnk) {
                strcpy(out_file_name, chnk);
            } else {
                fprintf(stderr, "Error redirecting 'out': no filename provided. Aborting\n");
                return -1;
            }
        } else if (strcmp(chnk, "<") == 0) {
            is_in_redirect = 1;
            chnk = strtok(NULL, " \t");

            if (chnk) {
                strcpy(in_file_name, chnk);
            } else {
                fprintf(stderr, "Error redirecting 'in': no filename provided. Aborting\n");
                return -1;
            }
        } else {
            cmd_args[cmd_len] = (char*) malloc(sizeof(char) * (strlen(chnk) + strlen(homedir) + 100));  // TODO: change size
            strcpy(cmd_args[cmd_len], chnk);
            cmd_len++;
        }

        chnk = strtok(NULL, " \t");
    }
    free(line);

    // for (int i = 0; i < cmd_len; i++) {
    //     printf("arg: %s || ", cmd_args[i]);
    // }
    // printf("\n");
    // printf("------ NOW HANDLING COMMAND-----------\n");

    // char* hist = (char*) malloc(sizeof(char) * HIST_STR_SIZE);
    // build_cmd(cmd, cmd_args, cmd_len, hist);
    // insert_history(hist);
    // free(hist);

    /*  redirection  */
    int hr_return = handle_redirection();

    int quit = 0;
    if (hr_return < 0) {
        quit = -1;
    } else if (!strcmp(cmd, "cd")) {
        quit = cd_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "echo")) {
        quit = echo_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "pwd")) {
        quit = pwd_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "ls")) {
        quit = ls_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "pinfo")) {
        quit = pinfo_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "history")) {
        quit = history_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "setenv")) {
        quit = setenv_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "unsetenv")) {
        quit = unsetenv_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "getenv")) {
        quit = test_getenv(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "jobs")) {
        quit = jobs_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "kjob")) {
        quit = kjob_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "bg")) {
        quit = bg_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "fg")) {
        quit = fg_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "overkill")) {
        quit = overkill_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "quit")) {
        quit = -2;
    } else {
        if (cmd_len >= 1 && strcmp(cmd_args[cmd_len-1], "&") == 0) {
            quit = bg_execution(cmd, cmd_args, cmd_len - 1);
        } else {
            quit = fg_execution(cmd, cmd_args, cmd_len);
        }
    }

    curr_fg_pid = -1;

    for (int i = 0; i < cmd_len; i++) {
        free(cmd_args[i]);
    }

    /* freedom */
    free(cmd_args);
    free(cmd);
    free(inp);

    clean_redirection();
    return quit;
}
