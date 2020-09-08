int fg_execution(char* cmd, char** cmd_args, const int arg_len);
int bg_execution(char* cmd, char** cmd_args, const int arg_len);
void update_bg_procs_sig();
void update_bg_procs();