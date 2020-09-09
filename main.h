#include <stdio.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/stat.h>
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

List * bg_procs;