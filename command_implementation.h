#include <grp.h>

#define COL_DEL " :\t"

int echo_implementation(char* cmd, char** cmd_args, const int arg_len);
int cd_implementation(char* cmd, char** cmd_args, const int arg_len);
int pwd_implementation(char* cmd, char** cmd_args, const int arg_len);
int ls_implementation(char* cmd, char** cmd_args, const int arg_len);
void get_perms(struct stat s, char *perms);
void prettify_time__(char* mod_time);
int ls(char* path, int flags[256], int print_name);
int pinfo_implementation(char* cmd, char** cmd_args, const int arg_len);
int system_cmd_implementation(char* cmd, char** cmd_args, const int arg_len);
int history_implementation(char* cmd, char** cmd_args, const int arg_len);
int kjob_implementation(char* cmd, char** cmd_args, const int arg_len);
int fg_implementation(char* cmd, char** cmd_args, const int arg_len);
int bg_implementation(char* cmd, char** cmd_args, const int arg_len);
int overkill_implementation(char* cmd, char** cmd_args, const int arg_len);