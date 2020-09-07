#include "sys_commands.h"
#include "main.h"
#include "command_implementation.h"
#include "misc.h"

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
        setpgid(0, 0);  // Why do this?
        system_cmd_implementation(cmd, cmd_args, arg_len);
        return 0;
    } else {
        char* temp = (char*) malloc(sizeof(char) * STR_SIZE);
        build_cmd(cmd, cmd_args, arg_len, temp);
        printf("CMD[%s] with PID[%d] started in background\n", temp, cpid);
        add(cpid, temp, bg_procs);
        free(temp);
        return 0;
    }
}

int proc_status(pid_t pid) {
    return kill(pid, 0);
}


// DOES NOT WORK, AT ALL
void update_bg_procs() {
    Node * curr = bg_procs->head;

    while (curr != NULL) {
        if (proc_status(curr->data) < 0) {
            printf("CMD[%s] with PID[%d] terminated\n", curr->cmd, curr->data);
            delete(curr->data, bg_procs);
        }
        curr = curr->next;
    }
}