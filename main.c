#include "main.h"
#include "io.h"
#include "command_implementation.h"
#include "parse_cmd.h"
#include "sys_commands.h"

int init() {
    bg_procs = makelist();
    signal(SIGCHLD, update_bg_procs_sig);
    homedir = malloc(sizeof(char) * STR_SIZE);
    if (getcwd(homedir, STR_SIZE) == NULL) {
        perror("Error getting cwd");
        return -1;
    }
    return 0;
}

void cleanup() {
    free(homedir);
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
            perror("Error getting input");
            return 1;
        };
        if (inp[strlen(inp)-1] == '\n')
            inp[strlen(inp)-1] = '\0';

        if (separate_cmd(inp) == -2)  /* Quit message encountered */
            break;

        fflush(stdin);
    }

    free(inp);
    cleanup();
    return 0;
}