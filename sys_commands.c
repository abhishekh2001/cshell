#include "sys_commands.h"
#include "main.h"
#include "command_implementation.h"
#include "misc.h"
#include "io.h"

int fg_execution(char* cmd, char** cmd_args, const int arg_len) {
    pid_t cpid;
    int status;

    cpid = fork();
    if (cpid == -1) {
        perror("Error trying to fork");
        return -1;
    }

   if (cpid == 0) {            /* Code executed by child */
        setpgid(0, 0);
        system_cmd_implementation(cmd, cmd_args, arg_len);
        exit(0);
    } else {                    /* Code executed by parent */

        /* Refer: https://stackoverflow.com/questions/5341220/how-do-i-get-tcsetpgrp-to-work-in-c */
        /*
            You need to call tcsetpgrp() in your parent process not in child
            Parent process will receive SIGTTOU and will be stopped.
        */
        signal(SIGTTOU, SIG_IGN);

        setpgid(cpid, 0);
        tcsetpgrp(STDIN_FILENO, cpid);

        waitpid(cpid, &status, WUNTRACED);

        tcsetpgrp(STDIN_FILENO, getpid());

        signal(SIGTTOU, SIG_DFL);
        return 0;
    }
}

int bg_execution(char* cmd, char** cmd_args, const int arg_len) {
    pid_t cpid, w;
    int status;

    cpid = fork();
    setpgid(0, 0);

    if (cpid < 0) {
        perror("Could not fork background process");
        return -1;
    }

    if (cpid == 0) {  // In child
        return system_cmd_implementation(cmd, cmd_args, arg_len);
    } else {
        char* temp = (char*) malloc(sizeof(char) * STR_SIZE);
        build_cmd(cmd, cmd_args, arg_len, temp);
        printf("CMD[%s] with PID[%d] started in background\n", temp, cpid);
        add(cpid, temp, bg_procs);
        free(temp);

        return 0;
    }
}

// Returns 0 if process in zombie or terminated state
int proc_status(pid_t pid) {
    char* proc_status_path = (char*) malloc(sizeof(char) * STR_SIZE);
    char process_status;

    sprintf(proc_status_path, "/proc/%d/status", pid);
    FILE *f_proc_status = fopen(proc_status_path, "r");
    if (f_proc_status == NULL) {
        perror("Error opening proc/<pid>/status to check state of background process");
        return -1;
    }

    char* line = (char*) malloc(sizeof(char) * (STR_SIZE + 1));  // free line
    while (fgets(line, STR_SIZE, f_proc_status) != NULL) {
        // line stores a single line from /proc/<pid>/status
        // cat /proc/2385/status

        char* word = strtok(line, COL_DEL);
        char* word_ = strtok(NULL, COL_DEL);
        if ((strcmp(word, "State") == 0) && word_) {
            process_status = word_[0];
        }
    }


    free(line);
    free(proc_status_path);
    if (process_status == 'Z' || process_status == 'T')
        return 0;
    return 1;
}


// naiive
void update_bg_procs() {
    Node * curr = bg_procs->head;
    int status;

    while (curr != NULL) {
        if (proc_status(curr->data) == 0) {
            printf("CMD[%s] with PID[%d] terminated\n", curr->cmd, curr->data);
            waitpid(curr->data, &status, WNOHANG);
            delete(curr->data, bg_procs);
        }
        curr = curr->next;
    }
}

void __handle_disp() {
    display_prompt();
    fflush(stdout);

    /*
    char* inp;
    if (getline (&inp, &temp, stdin) < 0) {
        perror("Error getting input");
        return 1;
    };
    if (inp[strlen(inp)-1] == '\n')
        inp[strlen(inp)-1] = '\0';
    separate_cmd(inp);
    */
}

void update_bg_procs_sig() {
    int status;
    pid_t cpid = waitpid(-1, &status, WNOHANG);
    if (!WIFEXITED(status)) {
        return;
    }

    printf("\n");
    for (Node * job = bg_procs->head; job != NULL; job = job->next) {
        printf("pid = %d\n", job->data);
        if (cpid == job->data) {
            printf("CMD[%s] having PID[%d] has terminated\n", job->cmd, job->data);

            __handle_disp();

            delete(cpid, bg_procs);
        }
    }
}