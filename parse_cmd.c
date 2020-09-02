#include "main.h"
#include "parse_cmd.h"
#include "command_implementation.h"
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
    char* cmd = (char*) malloc(sizeof(inp));
    char** cmd_args;                   
    cmd_args = (char**) malloc(sizeof(char**) * strlen(inp)+10);
    int cmd_len;

    tokenize(inp, cmd, cmd_args, &cmd_len);

    printf("After tokenizing, command extracted = %s\n", cmd);
    printf("Command number: %d\n", cmd_len);
    for (int i = 0; i < cmd_len; i++) {
        printf("arg: %s || ", cmd_args[i]);
    }
    printf("\n");

    printf("------ NOW HANDLING COMMANDS-----------\n");

    if (!strcmp(cmd, "cd")) {
        cd_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "echo")) {
        echo_implementation(cmd, cmd_args, cmd_len);
    } else if (!strcmp(cmd, "pwd")) {
        pwd_implementation(cmd, cmd_args, cmd_len);
    } else {
        printf("command not identified");
    }
    

    for (int i = 0; i < cmd_len; i++) {
        free(cmd_args[i]);
    }
    free(cmd_args);
    free(cmd);

    printf("Done freeing\n");
}