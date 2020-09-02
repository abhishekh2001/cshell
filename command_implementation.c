#include "main.h"
#include "command_implementation.h"
#include "misc.h"

// end with newline
int echo_implementation(char* cmd, char** cmd_args, const int arg_len) {
    for (int i = 0; i < arg_len; i++) {
        printf("%s ", cmd_args[i]);
    }
    printf("\n");
    return 0;
}

// ASSUMPTION: no spaces in path
// TODO: handle ~ flag
int cd_implementation(char* cmd, char** cmd_args, const int arg_len) {
    if (arg_len > 1) {
        printf("Incorrect number of arguments or space encountered\n");
        return -1;
    }

    char* path = malloc(sizeof(char) * STR_SIZE);
    if (arg_len == 0) {
        strcpy(path, "~");
    } else {
        handle_tilda(cmd_args[0], path);
    }

    printf("Handled path is now %s\n", path);

    chdir(path);
    return 0;
}

int pwd_implementation(char* cmd, char** cmd_args, const int arg_len) {
    char* path = malloc(sizeof(char) * STR_SIZE);
    if (getcwd(path, STR_SIZE) == NULL) {
        perror("Error getting cwd");
        return -1;
    }
    printf("%s\n", path);
    free(path);
    return 0;
}