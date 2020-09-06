#include "main.h"
#include "io.h"
#include "command_implementation.h"
#include "parse_cmd.h"

int main() {
    // struct passwd *pw = getpwuid(getuid());
    // homedir = pw->pw_dir;

    homedir = malloc(sizeof(char) * STR_SIZE);
    if (getcwd(homedir, STR_SIZE) == NULL) {
        perror("Error getting cwd");
        return -1;
    }

    size_t temp;
    char prompt[STR_SIZE], clean_inp[STR_SIZE];
    char* inp;
    inp = malloc(STR_SIZE);

    // loop for prompt
    while (1) {
        get_prompt(prompt);
        printf("%s ", prompt);
        
        // --------- RETRIEVE INPUT ----------
        // scanf(" %[^\n]s", inp);
        if (getline (&inp, &temp, stdin) < 0) {
            perror("Error getting input");
            return 1;
        };
        if (inp[strlen(inp)-1] == '\n')
            inp[strlen(inp)-1] = '\0';

        printf("Input: %s\n", inp);
        separate_cmd(inp);

        fflush(stdin);
    }

    free(homedir);
}