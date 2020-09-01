#include <stdio.h>
#include <sys/utsname.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <malloc.h>

#define STR_SIZE 1000000
#define SPACE " "

// convention for function pipes
// func(command, command args, number_args)

// assume command does not cross STR_SIZE
// Notes on allocating memory
// Malloc is left to be handled by the caller
// https://stackoverflow.com/questions/13442375/best-practice-for-allocating-memory-for-use-by-a-function-malloc-inside-or-out

// URGENT!!!! FREE CMD_ARGS -- How do I handle this?
// 1) have helper function that *must* be called
// tokenize works on one command
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
    if (arg_len != 1) {
        printf("Incorrect number of arguments or space encountered\n");
        return -1;
    }

    chdir(cmd_args[0]);
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

    temp_ch[0] = '<';
    strcpy (str, temp_ch);
    strcat(str, username); (temp_ch[0] = '@');
    strcat(str, temp_ch);
    strcat(str, val.sysname); temp_ch[0] = ':';
    strcat(str, temp_ch);
    strcat(str, path); temp_ch[0] = '>';
    strcat(str, temp_ch);
}

void handle_input(char* inp) {
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

int main() {
    size_t temp;
    char prompt[STR_SIZE], clean_inp[STR_SIZE];
    char* inp;
    inp = malloc(STR_SIZE);

    // loop for prompt
    while (1) {
        get_prompt(prompt);
        printf("%s ", prompt);
        
        // --------- RETRIEVE INPUT ----------
        // scanf(" %[^\n]s", inp);
        if (getline (&inp, &temp, stdin) < 0) {
            perror("Error getting input");
            return 1;
        };
        if (inp[strlen(inp)-1] == '\n')
            inp[strlen(inp)-1] = '\0';

        printf("Input: %s\n", inp);
        handle_input(inp);

        // handle cd
        // char* temp_clean_inp = malloc(STR_SIZE);
        // strcpy(temp_clean_inp, clean_inp);
        // if (strcmp(strtok(temp_clean_inp, " "), "cd") == 0) {
        //     cd_implementation(clean_inp);
        // } else {
        //     echo_implementation(clean_inp);
        // }
        fflush(stdin);
    }
}