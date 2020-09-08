#include "main.h"
#include "io.h"
#include "command_implementation.h"
#include "parse_cmd.h"
#include "sys_commands.h"

int main() {
    // struct passwd *pw = getpwuid(getuid());
    // homedir = pw->pw_dir;

    bg_procs = makelist();

    homedir = malloc(sizeof(char) * STR_SIZE);
    if (getcwd(homedir, STR_SIZE) == NULL) {
        perror("Error getting cwd");
        return -1;
    }

    size_t temp;
    char prompt[STR_SIZE], clean_inp[STR_SIZE];
    char* inp;
    inp = malloc(STR_SIZE);

    signal(SIGCHLD, update_bg_procs_sig);

    // loop for prompt
    while (1) {
        display_prompt();
        
        // --------- RETRIEVE INPUT ----------
        if (getline (&inp, &temp, stdin) < 0) {
            perror("Error getting input");
            return 1;
        };
        if (inp[strlen(inp)-1] == '\n')
            inp[strlen(inp)-1] = '\0';

        separate_cmd(inp);

        // update_bg_procs();

        fflush(stdin);
    }

    free(homedir);
    destroy(bg_procs);
}