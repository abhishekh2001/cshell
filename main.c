#include "main.h"
#include "io.h"
#include "command_implementation.h"
#include "parse_cmd.h"
#include "history.h"
#include "sys_commands.h"

// Main

int init() {
    cmd_exit_status = 111111;

    master_shell_pid = getpid();

    bg_procs = makelist();

    curr_fg_pid = -1;
    
    signal(SIGCHLD, update_bg_procs_sig);
    signal(SIGINT, sig_handler);
    signal(SIGTSTP, sig_handler);

    homedir = (char*)malloc(sizeof(char) * STR_SIZE);
    history_path = (char*) malloc(sizeof(char) * STR_SIZE);

    if (getcwd(homedir, STR_SIZE) == NULL) {
        perror("Error getting cwd");
        return -1;
    }

    strcpy(curr_working_directory, homedir);
    strcpy(last_working_directory, homedir);
    strcpy(history_path, homedir);
    strcat(history_path, "/.history.txt");

    int hd = open(history_path, O_CREAT | O_RDWR, 0777);

    close(hd);
    open_history();

    return 0;
}

void cleanup() {
    cleanup_history();
    free(homedir);
    free(history_path);
    printf("Killing all background processes\n");
    overkill_implementation("overkill", NULL, 0);
    destroy(bg_procs);

    printf("Quitting!\n");
}

int main() {
    if (init() < 0) {
        return -1;
    }

    size_t temp;
    char prompt[STR_SIZE], clean_inp[STR_SIZE];
    char* inp = (char*) malloc(sizeof(char) * STR_SIZE);

    // loop for prompt
    while (1) {
        display_prompt();

        /* --------- RETRIEVE INPUT ---------- */
        if (getline (&inp, &temp, stdin) < 0) {
            printf("\n");
            break;
        };

        if (!strcmp(inp, "\n")) {
            cmd_exit_status = 0;
            continue;
        }
        
        if (inp[strlen(inp)-1] == '\n')
            inp[strlen(inp)-1] = '\0';

        int rv = separate_cmd(inp);
        
        if (rv == -2)  /* Quit message encountered */
            break;
        else if (rv < 0)
            cmd_exit_status = -1;
        else
            cmd_exit_status = 0;

        fflush(stdin);
    }

    free(inp);
    cleanup();
    return 0;
}