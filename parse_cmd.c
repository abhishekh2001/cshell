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
    char* cmd = strtok(inp, ";");
    while (cmd != NULL) {
        printf("--COMMAND--: %s\n", cmd);

        char* temp = malloc(sizeof(char) * strlen(cmd));
        strcpy(temp, cmd);
        handle_cmd(temp);
        free(temp);

        cmd = strtok(NULL, ";");
    }
    printf("Done executing all commands\n");
}

void handle_cmd(char* inp) {
    char* cmd = (char*) malloc(sizeof(char) * strlen(inp)+1);
    char** cmd_args = (char**) malloc(sizeof(char*) * strlen(inp));
    int cmd_len = 0;

    // tokenize(inp, cmd, cmd_args, &cmd_len);


    // tokenize
    char* line = (char*) malloc(sizeof(char)*strlen(inp)+10);
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
    printf("Successfully alloted mem for **cmd_args\n");
    printf("Command copied %s\n", chnk);
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
        if (cmd_len > 1 && strcmp(cmd_args[cmd_len-1], "&") == 0) {
            // execute in the bg
        } else {
            fg_execution(cmd, cmd_args, cmd_len);
        }
    }

    for (int i = 0; i < cmd_len; i++) {
        free(cmd_args[i]);
    }
    free(cmd_args);
    free(cmd);

    printf("Done freeing\n");
}

