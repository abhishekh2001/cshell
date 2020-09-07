#include "main.h"
#include "parse_cmd.h"
#include "command_implementation.h"
#include "sys_commands.h"
#include "io.h"

// inp: semicolon separated inputs
// idea: char** commands
// split by ; and then store in array
// loop through array
// execute one by one
// so sed
void separate_cmd(char* inp) {
    printf("input given to separa cmd: %s\n", inp);
    unsigned long num_cmds = 0;

    char** cmd_arr = (char**) malloc(sizeof(char) * (strlen(inp) + 10));

    char* cmd = strtok(inp, ";");
    while (cmd != NULL) {
        printf("--COMMAND--: %s\n", cmd);
        cmd_arr[num_cmds] = (char*) malloc(sizeof(char) * (strlen(cmd) + 100));
        strcpy(cmd_arr[num_cmds++], cmd);

        cmd = strtok(NULL, ";");
    }

    for (int i = 0; i < num_cmds; i++) {
        handle_cmd(cmd_arr[i]);
        printf("-------- Done handling %s---------\n", cmd_arr[i]);
        free(cmd_arr[i]);
    }
    free(cmd_arr);
    printf("Done executing all commands\n");
}

void handle_cmd(char* inp_cmd) {
    printf("************** Handling input %s\n", inp_cmd);
    char* inp = (char*) malloc(sizeof(char) * (strlen(inp_cmd)));
    strcpy(inp, inp_cmd);
    char* cmd = (char*) malloc(sizeof(char) * (strlen(inp)+10));
    char** cmd_args = (char**) malloc(sizeof(char*) * strlen(inp));
    int cmd_len = 0;

    // tokenize(inp, cmd, cmd_args, &cmd_len);


    // tokenize
    char* line = (char*) malloc(sizeof(char)*(strlen(inp)+10));
    printf("Successfully created line\n");
    strcpy(line, inp);

    char* chnk = strtok(line, " \t");
    if (chnk == NULL) {
        printf("Abruptly exit tokenizing command\n");
        return;
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

    printf("After tokenizing, command extracted = %s\n", cmd);
    printf("Command number: %d\n", cmd_len);
    for (int i = 0; i < cmd_len; i++) {
        printf("arg: %s || ", cmd_args[i]);
    }
    printf("\n");

    printf("------ NOW HANDLING COMMAND-----------\n");

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
    }
    else {
        if (cmd_len >= 1 && strcmp(cmd_args[cmd_len-1], "&") == 0) {
            bg_execution(cmd, cmd_args, cmd_len - 1);
        } else {
            fg_execution(cmd, cmd_args, cmd_len);
        }
    }

    printf("Finished dealing with command. Time to free!\n");
    for (int i = 0; i < cmd_len; i++) {
        free(cmd_args[i]);
    }
    free(cmd_args);
    free(cmd);
    free(inp);
    printf("Done freeing\n");

    printf("Current background processes\n");
    update_bg_procs();
    display(bg_procs);
}
