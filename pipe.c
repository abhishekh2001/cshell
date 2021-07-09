#include "pipe.h"
#include "main.h"
#include "parse_cmd.h"
#include "history.h"

int mother_pipe(char *inp) {
    unsigned long num_cmds = 1;

    insert_history(inp);
    
    for (int i = 0; i < strlen(inp); i++) {
        if (inp[i] == '|')
            num_cmds++;
    }

    if (num_cmds == 1) {
        return handle_cmd(inp);
    }
    char** cmd_arr = (char**) malloc(sizeof(char*) * (num_cmds + 10));

    num_cmds = 0;
    char* cmd = strtok(inp, "|");
    while (cmd != NULL) {
        cmd_arr[num_cmds] = (char*) malloc(sizeof(char) * (strlen(cmd) + 100));
        strcpy(cmd_arr[num_cmds++], cmd);

        cmd = strtok(NULL, "|");
    }
    
    int quit = handle_pipe(cmd_arr, num_cmds);

    for (int i = 0; i < num_cmds; i++) {
        if (!strcmp(cmd_arr[i], "quit"))
            quit = -2;
        free(cmd_arr[i]);
    }
    free(cmd_arr);

    return quit;
}

int handle_pipe(char **cmd, int cmd_len) {
    int pipefd[2], curr_stdin, curr_stdout, pipe_stdin_copy, pipe_stdout_copy;
    pid_t cpid;

    /*  Setup file descriptors  */
    curr_stdin = STDIN_FILENO;
    curr_stdout = STDOUT_FILENO;

    if ((pipe_stdout_copy = dup(STDOUT_FILENO)) < 0) {
        perror("Error redirecting 'out'");
        return -1;
    }

    if ((pipe_stdin_copy = dup(STDIN_FILENO)) < 0) {
        perror("Error redirecting 'in' during pipe");
        return -1;
    }

    int status, rv;
    
    /* MAIN LOOP LOGIC */
    // Go through commands
    // Keep changing STDOUT and STDIN
    for (int i = 0; i < cmd_len; i++) {
        if (pipe(pipefd) < 0) {
            perror("Unable to open pipe");
            fprintf(stderr, "Aborting command sequences: Pipe failed.\n");
            return -1;
        }

        if ((cpid = fork()) < 0) {
            perror("Fork failed during pipe");
            fprintf(stderr, "Aborting command sequenceses: forking for pipe failed.\n");
            return -1;
        }

        if (cpid == 0) {  /*  in child  */
            if (dup2(curr_stdin, STDIN_FILENO) < 0) {
                perror("Error while piping");
                return -1;
            }
            
            if (i != cmd_len-1 && dup2(pipefd[1], STDOUT_FILENO) < 0) {
                perror("Error while piping");
                return -1;
            }

            if (i != cmd_len-1)
                dup2(pipefd[1], 1);

            close(pipefd[0]);

            if (handle_cmd(cmd[i]) < 0)
                exit(EXIT_FAILURE);
            exit(EXIT_SUCCESS);
            
        } else {  /*  in parent  */
            waitpid(cpid, &status, WUNTRACED);
            close(pipefd[1]);
            curr_stdin = pipefd[0];
        }
    }

    if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)
        rv = 0;
    else
        rv = -1;

    return rv;
}