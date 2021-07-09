#include "env.h"
#include "main.h"

int setenv_implementation(char* cmd, char** cmd_args, const int arg_len) {
    if (arg_len <= 0 || arg_len > 2) {
        fprintf(stderr, "Error while setenv: incorrect number of arguments\n");
        return -1;
    }

    char val[100000];
    if (arg_len == 1) {
        strcpy(val, "");
    } else {
        strcpy(val, cmd_args[1]);
    }

    if (setenv(cmd_args[0], val, 1) < 0) {
        perror("Error at setenv");
        return -1;
    }
    return 0;
}

int unsetenv_implementation(char* cmd, char** cmd_args, const int arg_len) {
    if (arg_len == 0 || arg_len > 1) {
        fprintf(stderr, "Error at unsetenv: Illegal number of arguments\n");
        return -1;
    }

    if (unsetenv(cmd_args[0]) < 0) {
        perror("Error at unsetenv");
        return -1;
    }

    return 0;
}

int test_getenv(char* cmd, char** cmd_args, const int arg_len) {
    if (arg_len == 0) {
        return -1;
    }
    if (getenv(cmd_args[0])) {
        printf("%s\n", getenv(cmd_args[0]));
        return 0;
    } else {
        printf("env var %s does not exist\n", cmd_args[0]);
        return -1;
    }

    return 0;
}