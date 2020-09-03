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
        strcpy(path, homedir);
    } else {
        handle_tilda(cmd_args[0], path);
    }

    chdir(path);
    return 0;
}

// Prints current working directory
// Flags?
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

// Highest function
int ls_implementation(char* cmd, char** cmd_args, const int arg_len) {
    int flags[256] = { 0 };
    int opt, cmd_args_len, num_dirs = 0, print_file_names = 0;

    for (int i = 0; i < arg_len; i++) {
        if (cmd_args[i][0] == '-') {
            cmd_args_len = strlen(cmd_args[i]);

            if (cmd_args_len == 1) {
                // call ls on directory '-'
                num_dirs++;
            }
            for (int j = 1; j < cmd_args_len; j++) {
                opt = cmd_args[i][j];
                if (opt != 'l' && opt != 'a') {
                    printf("Invalid option '%c' encountered\n", opt);
                    return -1;
                }
                flags[opt] = 1;
            }
        }
    }

    for (int i = 0; i < arg_len; i++) {
        if (cmd_args[i][0] != '-') {
            num_dirs++;
        }
    }

    printf("numdirs encountered = %d\n", num_dirs);
    if (num_dirs == 0) {
        char* cwd = malloc(sizeof(char) * STR_SIZE);
        if (getcwd(cwd, STR_SIZE) == NULL) {
            perror("Error getting cwd");
            return -1;
        }
        ls(cwd, flags);
        free(cwd);
        return 0;
    } else if (num_dirs == 1) {
        print_file_names = 0;
    } else {
        print_file_names = 1;
    }

    for (int i = 0; i < arg_len; i++) {
        if ((cmd_args[i][0] == '-' && strlen(cmd_args[i]) == 1) || cmd_args[i][0] != '-') {
            if (print_file_names) {
                printf("%s:\n", cmd_args[i]);
            }
            ls(cmd_args[i], flags);
            printf("\n");
        }
    }

    printf("l: %d, a: %d\n", flags['l'], flags['a']);
    return 0;
}

// inner implementation handling the printing
int ls(char* path, int flags[256]) {
    struct stat pstat;

    char* ptemp = (char*) malloc(sizeof(char) * STR_SIZE);
    strcpy(ptemp, path); handle_tilda(ptemp, path);
    free(ptemp);

    printf("Calling path %s\n", path);
    // if ((stat(path, &pstat) < 0) || !S_ISDIR(pstat.st_mode)) {
    if ((stat(path, &pstat) < 0)) {  // TODO: check for directory
        perror("Could not list directory contents");
        return -1;
    }


    DIR *dir;
    struct dirent *ent;

    if (!flags['l']) {
        if ((dir = opendir(path)) != NULL) {
            /* print all the files and directories within directory */
            while ((ent = readdir(dir)) != NULL) {
                if (ent->d_name[0] == '.' && !flags['a'])
                    continue;
                printf("%s\t", ent->d_name);
            }
            closedir(dir);
        } else {
            /* could not open directory */
            perror("Could not open directory");
            return -1;
        }
    } else {
        printf("TODO: handle list formatting");
    }
}

