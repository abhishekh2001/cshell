#include <stdio.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/wait.h>
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

#define STR_SIZE 100000
#define SPACE " "

char *homedir;

List * bg_procs;