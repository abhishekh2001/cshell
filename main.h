#include <stdio.h>
#include <sys/utsname.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <malloc.h>
#include <dirent.h>

#define STR_SIZE 100000
#define SPACE " "

char *homedir;
