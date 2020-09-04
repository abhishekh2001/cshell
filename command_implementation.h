#include <grp.h>

int echo_implementation(char* cmd, char** cmd_args, const int arg_len);
int cd_implementation(char* cmd, char** cmd_args, const int arg_len);
int pwd_implementation(char* cmd, char** cmd_args, const int arg_len);
int ls_implementation(char* cmd, char** cmd_args, const int arg_len);
void get_perms(struct stat s, char *perms);
void prettify_time__(char* mod_time);
int ls(char* path, int flags[256]);