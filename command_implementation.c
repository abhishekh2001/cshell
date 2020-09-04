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
        ls(cwd, flags, 0);
        free(cwd);
        return 0;
    } else if (num_dirs == 1) {
        print_file_names = 0;
    } else {
        print_file_names = 1;
    }

    for (int i = 0; i < arg_len; i++) {
        if ((cmd_args[i][0] == '-' && strlen(cmd_args[i]) == 1) || cmd_args[i][0] != '-') {
            ls(cmd_args[i], flags, print_file_names);
            printf("\n");
        }
    }

    printf("l: %d, a: %d\n", flags['l'], flags['a']);
    return 0;
}

void get_perms(struct stat s, char *perms) {
    perms[0] = '-';

    if (S_ISLNK(s.st_mode))
        perms[0] = 'l';
    else if (S_ISDIR(s.st_mode))
        perms[0] = 'd';
    
    // usr
    perms[1] = (s.st_mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (s.st_mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (s.st_mode & S_IXUSR) ? 'x' : '-';

    // grp
    perms[4] = (s.st_mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (s.st_mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (s.st_mode & S_IRGRP) ? 'x' : '-';

    // oth
    perms[7] = (s.st_mode & S_IROTH) ? 'r' : '-';
    perms[8] = (s.st_mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (s.st_mode & S_IXOTH) ? 'x' : '-';

    perms[10] = '\0';
}

void prettify_time__(char* mod_time) {
    char* temp = malloc(sizeof(char) * strlen(mod_time));

    for (int i = 4; i < 20; i++)
        temp[i-4] = mod_time[i];
    temp[16] = '\0';
    strcpy(mod_time, temp);
    free(temp);
}

// inner implementation handling the printing
int ls(char* path, int flags[256], int print_name) {
    struct stat pstat, s;

    char* ptemp = (char*) malloc(sizeof(char) * STR_SIZE);
    strcpy(ptemp, path);
    handle_tilda(ptemp, path);
    free(ptemp);

    if (print_name) {
        printf("%s:\n", path);
    }
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
        if ((dir = opendir(path)) != NULL) {
            // TODO: retrieve total_blocks prior to listing content
            blkcnt_t total_blocks = 0;
            nlink_t n_links;
            off_t  f_size;
            char perms[30];
            char* mod_time;
            /* print all the files and directories within directory */
            while ((ent = readdir(dir)) != NULL) {
                if (ent->d_name[0] == '.' && !flags['a'])
                    continue;

                char* filepath = (char*) malloc(sizeof(char) * (strlen(path) + strlen(ent->d_name) + 1));  // FREE
                strcpy(filepath, path);
                strcat(filepath, "/");
                strcat(filepath, ent->d_name);

                if (stat(filepath, &s) < 0) {
                    perror("Unable to retrieve file");
                    free(filepath);
                    continue;
                }

                // Get file information
                // -rwxrwxr-x 1 usr grp  16696 Sep  3 00:31
                get_perms(s, perms);
                n_links = s.st_nlink;
                char* user = getpwuid(s.st_uid)->pw_name;
                char* group = getgrgid(s.st_gid)->gr_name;
                f_size = s.st_size;
                mod_time = ctime(&s.st_mtime);

                prettify_time__(mod_time);

                total_blocks += s.st_blocks;

                printf("%s\t%lu\t%s\t%s\t%lu\t%s\t%s\n",
                    perms, n_links, user, group, f_size,
                    mod_time, ent->d_name);

                free(filepath);
            }
            printf("total: %ld\n", total_blocks / 2);  // Check later why div_2?
            closedir(dir);
        } else {
            /* could not open directory */
            perror("Could not open directory");
            return -1;
        }
    }
}

