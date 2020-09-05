#include "sys_commands.h"
#include "main.h"
#include "command_implementation.h"

int fg_execution(char* cmd, char** cmd_args, const int arg_len) {
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
            w = waitpid(cpid, &status, WUNTRACED);
            if (w == -1) {
                perror("waitpid");
                return -1;
            }

        //    if (WIFEXITED(status)) {
        //         printf("exited, status=%d\n", WEXITSTATUS(status));
        //     } else if (WIFSIGNALED(status)) {
        //         printf("killed by signal %d\n", WTERMSIG(status));
        //     } else if (WIFSTOPPED(status)) {
        //         printf("stopped by signal %d\n", WSTOPSIG(status));
        //     } else if (WIFCONTINUED(status)) {
        //         printf("continued\n");
        //     }
        } while (!WIFEXITED(status));
        return 0;
    }
}

int bg_execution(char* cmd, char** cmd_args, const int arg_len) {
    pid_t cpid, w;
    int status;

    cpid = fork();

    if (cpid < 0) {
        perror("Could not fork background process");
        return -1;
    }

    if (cpid == 0) {  // In child
        setpgid(0, 0);
        system_cmd_implementation(cmd, cmd_args, arg_len);
        return 0;
    } else {
        printf("Started process %d in the background\n", cpid);
        return 0;
    }
}