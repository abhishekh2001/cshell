#include "main.h"
#include "command_implementation.h"
#include "sys_commands.h"
#include "misc.h"
#include "history.h"

// end with newline
int echo_implementation(char* cmd, char** cmd_args, const int arg_len) {
    for (int i = 0; i < arg_len; i++) {
        printf("%s ", cmd_args[i]);
    }
    printf("\n");
    return 0;
}

// ASSUMPTION: no spaces in path
int cd_implementation(char* cmd, char** cmd_args, const int arg_len) {
    if (arg_len > 1) {
        fprintf(stderr, "Incorrect number of arguments or space encountered\n");
        return -1;
    }

    char* path = malloc(sizeof(char) * STR_SIZE);
    if (arg_len == 1 && strcmp(cmd_args[0], "-") == 0) {
        strcpy(path, last_working_directory);
        char* temp = (char*) malloc(sizeof(char) * strlen(last_working_directory));
        strcpy(temp, path);
        insert_tilda(temp);
        printf("%s\n", temp);
        free(temp);
    } else if (arg_len == 0) {
        strcpy(path, homedir);
    } else {
        handle_tilda(cmd_args[0], path);
        // strcpy(last_working_directory, path);
    }

    if (chdir(path) < 0) {
        perror("Error changing directories");
        return -1;
    }
    if (getcwd(path, STR_SIZE) == NULL) {
        perror("Error getting cwd");
        return -1;
    }
    strcpy(last_working_directory, curr_working_directory);
    strcpy(curr_working_directory, path);

    free(path);
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
    int rv = 0;
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
                    fprintf(stderr, "ls error: Invalid option '%c' encountered\n", opt);
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

    if (num_dirs == 0) {
        char* cwd = malloc(sizeof(char) * STR_SIZE);
        if (getcwd(cwd, STR_SIZE) == NULL) {
            perror("Error getting cwd");
            return -1;
        }
        ls(cwd, flags, 0);
        printf("\n");
        free(cwd);
        return 0;
    } else if (num_dirs == 1) {
        print_file_names = 0;
    } else {
        print_file_names = 1;
    }

    for (int i = 0; i < arg_len; i++) {
        if ((cmd_args[i][0] == '-' && strlen(cmd_args[i]) == 1) || cmd_args[i][0] != '-') {
            int temp_rv = ls(cmd_args[i], flags, print_file_names);
            rv = min(rv, temp_rv);
            printf("\n");
        }
    }
    
    return rv;
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
    perms[6] = (s.st_mode & S_IXGRP) ? 'x' : '-';

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
                printf("%s\n", ent->d_name);
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

                char* filepath = (char*) malloc(sizeof(char) * STR_SIZE);  // FREE
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
            if (closedir(dir) < 0) {
                perror("Error closing dir");
                return -1;
            }
        } else {
            /* could not open directory */
            perror("Could not open directory");
            return -1;
        }
    }
    return 0;
}

int pinfo_implementation(char* cmd, char** cmd_args, const int arg_len) {
    pid_t pid;
    ssize_t p_exec_path_len;
    char process_status;
    char process_status_path[STR_SIZE];
    char process_mem[STR_SIZE], process_exec_path[STR_SIZE+1], pid_str[STR_SIZE];

    if (arg_len == 0) {
        pid = getpid();
        sprintf(pid_str, "%d", pid);
    } else if (arg_len > 1) {
        fprintf(stderr, "Error at pinfo: invalid number of arguments\n");
        return -1;
    } else {
        for (int i = 0; i < strlen(cmd_args[0]); i++) {
            if (cmd_args[0][i] < '0' || cmd_args[0][i] > '9') {
                fprintf(stderr, "Error at pinfo: Illegal argument\n");
                return -1;
            }
        }
        if ((pid = strtol(cmd_args[0], NULL, 10)) < 0) {
            fprintf(stderr, "Error at pinfo: pid cannot be negative\n");
            return -1;
        }
        strcpy(pid_str, cmd_args[0]);
    }

    // process path: /proc/<pid>/status
    strcpy(process_status_path, "/proc/");
    strcat(process_status_path, pid_str);
    strcat(process_status_path, "/status");
    // process exec path: /proc/<pid>/exe
    strcpy(process_exec_path, "/proc/");
    strcat(process_exec_path, pid_str);
    strcat(process_exec_path, "/exe");

    FILE* f_process_status = fopen(process_status_path, "r");
    if (f_process_status == NULL) {
        perror("Error opening process status file");
        return -1;
    }

    char* line = (char*) malloc(sizeof(char) * (STR_SIZE + 1));  // free line
    int line_number = 0;
    while (fgets(line, STR_SIZE, f_process_status) != NULL) {
        // line stores a single line from /proc/<pid>/status
        // cat /proc/2385/status

        line_number++;
        char* word = strtok(line, COL_DEL);
        char* word_ = strtok(NULL, COL_DEL);
        if ((strcmp(word, "State") == 0 || line_number == 3) && word_) {
            process_status = word_[0];
        } else if ((strcmp(word, "VmSize") == 0 || line_number == 18) && word_) {
            strcpy(process_mem, word_);
        }
    }


    printf("pid -- %s\n", pid_str);
    printf("Process Status -- %c\n", process_status);
    printf("memory -- %s\n", process_mem);

    if ((p_exec_path_len = readlink(process_exec_path, line, STR_SIZE)) < 0) {
        perror("Accessing proc/<pid>/exe");
        return -1;
    }
    line[p_exec_path_len] = '\0';

    insert_tilda(line);

    printf("Executable Path -- %s\n", line);

    free(line);
    fclose(f_process_status);
    return 0;
}

int system_cmd_implementation(char* cmd, char** cmd_args, const int arg_len) {
    char** ex_args = (char**) malloc(sizeof(char*) * (arg_len + 1));
    ex_args[0] = (char*) malloc(sizeof(char) * strlen(cmd));
    strcpy(ex_args[0], cmd);
    
    for (int i = 0; i < arg_len; i++) {
        ex_args[i+1] = (char*) malloc(sizeof(char) * strlen(cmd_args[i]));
        strcpy(ex_args[i+1], cmd_args[i]);
    }
    ex_args[arg_len+1] = NULL;

    // printf("cmd: %s\n", cmd);
    // for (int i = 0; i <= arg_len; i++)
    //     printf("%s - ", ex_args[i]);
    // printf("\n");

    // printf("Before exec\n");
    if (execvp(cmd, ex_args) < 0) {
        perror("Error executing command system command");
        for (int i = 0; i <= arg_len; i++) {
            free(ex_args[i]);
        }
        free(ex_args);
        return -1;
    }

    // printf("Came out\n");

    for (int i = 0; i <= arg_len; i++) {
        free(ex_args[i]);
    }
    free(ex_args);
    
    return 0;
}

int history_implementation(char* cmd, char** cmd_args, const int arg_len) {
    if (arg_len == 0) {
        disp_history(10);
    } else if (arg_len > 1) {
        fprintf(stderr, "Incorrect command format: \"history [<num>]\"\n");
        return -1;
    } else {
        disp_history(strtol(cmd_args[0], NULL, 10));
    }
    return 0;
}

int kjob_implementation(char* cmd, char** cmd_args, const int arg_len) {
    if (arg_len != 2) {
        fprintf(stderr, "Error while kjob: Illegal number of arguments\n");
        return -1;
    }

    int sid = strtol(cmd_args[0], NULL, 10), signum = strtol(cmd_args[1], NULL, 10);

    Node * proc = get_proc_shell_id(sid, bg_procs);
    if (proc == NULL) {
        fprintf(stderr, "Error while kjob: no process found\n");
        return -1;
    }

    if (kill(proc->data, signum) < 0) {
        perror("Error while kjob");
        return -1;
    }

    return 0;
}

int fg_implementation(char* cmd, char** cmd_args, const int arg_len) {
    if (arg_len != 1) {
        fprintf(stderr, "Error while fg: Illegal number of args\n");
        return -1;
    }

    int shell_id = strtol(cmd_args[0], NULL, 10);
    if (shell_id < 0) {
        fprintf(stderr, "Error while fg: Illegal job number\n");
        return -1;
    }

    Node * proc = get_proc_shell_id(shell_id, bg_procs);
    int status;

    if (proc == NULL) {
        fprintf(stderr, "Error while fg: No such process\n");
        return -1;
    }

    char* temp_cmd = (char*) malloc(sizeof(char) * strlen(proc->cmd));
    pid_t cpid = proc->data;
    int temp_sid = proc->shell_id;
    strcpy(temp_cmd, proc->cmd);
    delete(proc->data, bg_procs);

    /*  set current fg proc  */
    curr_fg_pid = cpid;

    printf("CMD[%s] with PID[%d] and shell pid [%d] started in foreground\n", temp_cmd, cpid, temp_sid);

    /*  handle terminal control over to child  */
    signal(SIGTTOU, SIG_IGN);

    tcsetpgrp(STDIN_FILENO, cpid);

    kill(cpid, SIGCONT);

    waitpid(cpid, &status, WUNTRACED);

    tcsetpgrp(STDIN_FILENO, getpgrp());

    signal(SIGTTOU, SIG_DFL);

    /*  cleanup  */
    curr_fg_pid = -1;

    int rv = cleanup_fg_execution(temp_cmd, NULL, 0, cpid, status);

    if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)
        rv = min(rv, 0);
    else
        rv = -1;

    free(temp_cmd);

    return rv;

    return rv;
}

int bg_implementation(char* cmd, char** cmd_args, const int arg_len) {
    if (arg_len != 1) {
        fprintf(stderr, "Error while bg: Illegal number of arguments\n");
        return -1;
    }

    Node * proc = get_proc_shell_id(strtol(cmd_args[0], NULL, 10), bg_procs);

    if (proc == NULL) {
        fprintf(stderr, "Error while bg: No such process found\n");
        return -1;
    }

    if (kill(proc->data, SIGCONT) < 0) {
        perror("Error while bg");
        return -1;
    }

    return 0;
}

// TODO OVERKILL
int overkill_implementation(char* cmd, char** cmd_args, const int arg_len) {
    if (arg_len != 0) {
        fprintf(stderr, "Error while overkill: Illegal number of arguments\n");
        return -1;
    }
    Node * current = bg_procs->head;
    Node * next = current;
    while(current != NULL) {
        next = current->next;
        kill(current->data, 9);
        current = next;
    }

    return 0;
}