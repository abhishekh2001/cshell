#define HIST_STR_SIZE 100000

void open_history();
void disp_history(int num);
void cleanup_history();
void insert_history(char* cmd);
void load_history();

int enable_history, hist_ptr;
char history[20][HIST_STR_SIZE];