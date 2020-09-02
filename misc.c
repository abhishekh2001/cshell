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