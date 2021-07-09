#include <unistd.h>

int cleanup_fg_execution(char* cmd, char** cmd_args, const int arg_len, pid_t cpid, int status);
int fg_execution(char* cmd, char** cmd_args, const int arg_len);
int bg_execution(char* cmd, char** cmd_args, const int arg_len);
void update_bg_procs_sig();
int jobs_implementation(char* cmd, char** cmd_args, const int arg_len);
void update_bg_procs();
void sig_handler(int signum);
int is_stopped(int pid);