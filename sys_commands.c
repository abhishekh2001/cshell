#include "sys_commands.h"
#include "main.h"
#include "command_implementation.h"

int fg_execution(char* cmd, char** cmd_args, const int arg_len) {
    // int status;
    // pid_t pid = fork();

    // printf("Executing command %s in the foreground\n", cmd);

    // if (pid < 0) {
    //     printf("Error executing system call in the foreground: unable to fork\n");
    //     return -1;
    // } else if (pid == 0) {
    //     // In child
    //     setpgid(0, 0);
    //     system_cmd_implementation(cmd, cmd_args, arg_len);
    //     return 0;
    // } else {
    //     waitpid(pid, &status, 0);
    // }



    pid_t cpid, w;
    int status;

    cpid = fork();
    if (cpid == -1) {
        perror("Error trying to fork");
        return -1;
    }

   if (cpid == 0) {            /* Code executed by child */
        setpgid(0, 0);
        system_cmd_implementation(cmd, cmd_args, arg_len);
        return 0;
   } else {                    /* Code executed by parent */
        do {
            printf("Waiting\n");
            w = waitpid(cpid, &status, WUNTRACED);
            if (w == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }

           if (WIFEXITED(status)) {
                printf("exited, status=%d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("killed by signal %d\n", WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
                printf("stopped by signal %d\n", WSTOPSIG(status));
            } else if (WIFCONTINUED(status)) {
                printf("continued\n");
            }
        } while (!WIFEXITED(status));
        printf("Done waiting\n");
        return 0;
    }
}


