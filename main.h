#include <stdio.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <malloc.h>
#include <dirent.h>
#include <signal.h>
#include <time.h>
#include <pwd.h>
#include "linkedlist.h"

#define min(a, b) ((a < b) ? a : b)
#define max(a, b) ((a > b) ? a : b)
#define STR_SIZE 100000
#define SPACE " "

char *homedir;
char *history_path;

/*  Redirection  */

int file_in, stdin_copy, file_out, stdout_copy;
int is_out_redirect, is_in_redirect, is_append;
char out_file_name[10000], in_file_name[10000];
char last_working_directory[STR_SIZE], curr_working_directory[STR_SIZE];

/*  process  */
pid_t master_shell_pid, curr_fg_pid;
char curr_fg_name[100000];
int cmd_exit_status;
List * bg_procs;