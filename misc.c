#include "main.h"
#include "misc.h"

void handle_tilda(char* path, char* res) {
    if (strcmp(path, "~") == 0) {
        strcpy(res, homedir);
    } else if (path[0] == '~') {
        strcpy(res, homedir);
        strcat(res, path+1);
    } else {
        strcpy(res, path);
    }
}

void insert_tilda(char* path) {
    char* res = malloc(sizeof(char) * strlen(path));
    size_t path_len = strlen(path), home_len = strlen(homedir);
    int p, h;  // pos for path and homedir

    for (p = 0, h = 0; p < path_len && h < home_len; p++, h++) {
        if (path[p] != homedir[h]) {
            break;
        }
    }

    if (p == home_len) {
        strcpy(res, "~");
        strcat(res, path+home_len);
        strcpy(path, res);
    }

    free(res);
}

// char* res malloced by caller
void build_cmd(char* cmd, char** cmd_args, const int arg_len, char* res) {
    strcpy(res, cmd);
    for (int i = 0; i < arg_len; i++) {
        strcat(res, " ");
        strcat(res, cmd_args[i]);
    }
}
