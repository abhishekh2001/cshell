#include "main.h"
#include "io.h"
#include "misc.h"

int tokenize(char* raw_input, char* command, char** cmd_args, int* command_length) {
    int cmd_len = 0;

    char* line = (char*) malloc(sizeof(char)*strlen(raw_input)+10);
    printf("Successfully created line\n");

    strcpy(line, raw_input);

    char* cmd = strtok(line," \t");

    if (cmd == NULL) {
        printf("Abruptly exit tokenizing command\n");
        return -1;
    }
    strcpy(command, cmd);
    
    // should be alloted by caller
    // cmd_args = (char**) malloc(sizeof(char**) * strlen(raw_input)+1);
    printf("Successfully alloted mem for **cmd_args\n");

    printf("Command copied %s\n", command);
    cmd = strtok(NULL, " \t");
    while (cmd != NULL) {
        cmd_args[cmd_len] = (char*) malloc(sizeof(char) * strlen(cmd));
        strcpy(cmd_args[cmd_len], cmd);
        cmd_len++;

        cmd = strtok(NULL, " \t");
    }

    *command_length = cmd_len;

    free(line);
    return 0;
}

int get_prompt(char* str) {
    struct utsname val;
    char username[STR_SIZE], path[STR_SIZE], temp_ch[2];
    temp_ch[1] = '\0';

    if (gethostname(username, STR_SIZE) < 0) {
        perror("Error retrieving hostname");
        return -1;
    }
    if (uname(&val) < 0) {
        perror("Failed to retrieve user details");
        return -1;
    }
    if (getcwd(path, STR_SIZE) == NULL) {
        perror("Error getting cwd");
        return -1;
    }
    if (str == 0) {
        return -1;
    }

    insert_tilda(path);
    printf("With tilda: %s\n", path);

    temp_ch[0] = '<';
    strcpy (str, temp_ch);
    strcat(str, username); (temp_ch[0] = '@');
    strcat(str, temp_ch);
    strcat(str, val.sysname); temp_ch[0] = ':';
    strcat(str, temp_ch);
    strcat(str, path); temp_ch[0] = '>';
    strcat(str, temp_ch);
}