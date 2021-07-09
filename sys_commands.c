#include "sys_commands.h"
#include "main.h"
#include "command_implementation.h"
#include "misc.h"
#include "io.h"


char proc_status(pid_t pid) {
    char* proc_status_path = (char*) malloc(sizeof(char) * STR_SIZE);
    char process_status;

    sprintf(proc_status_path, "/proc/%d/status", pid);
    FILE *f_proc_status = fopen(proc_status_path, "r");
    if (f_proc_status == NULL) {
        perror("Error opening proc/<pid>/status to check state of background process");
        return 'O';  //Fatal error
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
    return process_status;
    // if (process_status == 'Z' || process_status == 'T')
    //     return 0;
    // return 1;
}

char proc_status_silent(pid_t pid) {
    char* proc_status_path = (char*) malloc(sizeof(char) * STR_SIZE);
    char process_status;

    sprintf(proc_status_path, "/proc/%d/status", pid);
    FILE *f_proc_status = fopen(proc_status_path, "r");
    if (f_proc_status == NULL) {
        // perror("Error opening proc/<pid>/status to check state of background process");
        return 'O';  //Fatal error
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
    return process_status;
    // if (process_status == 'Z' || process_status == 'T')
    //     return 0;
    // return 1;
}

// Recieves a process id and returns True if process is stopped
// and false if otherwise.
int is_stopped(int pid) {
    int status;
    waitpid(pid, &status, WNOHANG);
    return WIFSTOPPED(status);
    // return proc_status_silent(pid) == 'T';
}


// status must be the result of wait call
// with the WUNTRACED flag
// https://man7.org/linux/man-pages/man2/wait.2.html
// WIFSTOPPED(status)
// returns true if the child process was stopped by delivery of a
// signal; this is possible only if the call was done using WUN‐
// TRACED or when the child is being traced (see ptrace(2))
int cleanup_fg_execution(char* cmd, char** cmd_args, const int arg_len, pid_t cpid, int status) {
    if (WIFSTOPPED(status)) {
        char* temp = (char*) malloc(sizeof(char) * STR_SIZE);
        build_cmd(cmd, cmd_args, arg_len, temp);
        int sid = add(cpid, temp, bg_procs);
        printf("CMD[%s] with PID[%d] and shell pid [%d] started in background\n", temp, cpid, sid);
        free(temp);
        return -1;
    }
    return 0;
}

int fg_execution(char* cmd, char** cmd_args, const int arg_len) {
    pid_t cpid;
    int status;

    cpid = fork();
    if (cpid == -1) {
        perror("Error trying to fork");
        return -1;
    }

   if (cpid == 0) {            /* Code executed by child */
        // setpgid(0, 0);
        if (system_cmd_implementation(cmd, cmd_args, arg_len) < 0)
            exit(EXIT_FAILURE);
        exit(EXIT_SUCCESS);
    } else {                    /* Code executed by parent */

        /* Refer: https://stackoverflow.com/questions/5341220/how-do-i-get-tcsetpgrp-to-work-in-c */
        /*
            You need to call tcsetpgrp() in your parent process not in child
            Parent process will receive SIGTTOU and will be stopped.
        */

        curr_fg_pid = cpid;

        signal(SIGTTOU, SIG_IGN);

        setpgid(cpid, 0);
        tcsetpgrp(STDIN_FILENO, cpid);

        waitpid(cpid, &status, WUNTRACED);

        tcsetpgrp(STDIN_FILENO, getpgrp());

        signal(SIGTTOU, SIG_DFL);

        int rv = cleanup_fg_execution(cmd, cmd_args, arg_len, cpid, status);

        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)
            rv = min(rv, 0);
        else
            rv = -1;

        return rv;
    }
}

int bg_execution(char* cmd, char** cmd_args, const int arg_len) {
    pid_t cpid;
    int status;

    cpid = fork();
    setpgid(0, 0);

    if (cpid < 0) {
        perror("Could not fork background process");
        return -1;
    }

    if (cpid == 0) {  // In child
        if (system_cmd_implementation(cmd, cmd_args, arg_len) < 0) {
            printf("Killing cpid = %d\n", getpid());
            kill(getpid(), SIGKILL);  // WAS SIGKILL
            return -1;
        }
        exit(0);
    } else {
        char* temp = (char*) malloc(sizeof(char) * STR_SIZE);
        build_cmd(cmd, cmd_args, arg_len, temp);
        int sid = add(cpid, temp, bg_procs);
        printf("CMD[%s] with PID[%d] and shell pid [%d] started in background\n", temp, cpid, sid);
        free(temp);

        return 0;
    }
}

/*      jobs        */


int disp_bg_procs() {
    Node * current = bg_procs->head;
    if(bg_procs->head == NULL) 
        return -1;
    
    for(; current != NULL; current = current->next) {
        char status = proc_status(current->data);
        printf("[%d] %s %s [%d]\n", current->shell_id, (status == 'S' || status == 'R') ? "Running" : "Stopped", current->cmd, current->data);
    }
    return 0;
}

int jobs_implementation(char* cmd, char** cmd_args, const int arg_len) {
    if (arg_len != 0) {
        fprintf(stderr, "Error at jobs: Illegal arguments\n");
        return -1;
    }
    if (disp_bg_procs() < 0) {
        printf("No jobs\n");
    }
    return 0;
}

/*      done jobs       */

// Returns 0 if process in zombie or terminated state


/*      OUTDATED        */
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
    // signal(SIGTTIN, SIG_DFL);
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
    int status, successful;
    pid_t cpid;
    while (1) {
        successful = 1;
        cpid = waitpid(-1, &status, WNOHANG);

        if (cpid <= 0) {
            return;
        }

        printf("\n");
        Node * job = get_proc(cpid, bg_procs);
        if (!job) {
            // This is fine...
        } else {
            if (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS)
                successful = 0;
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                fprintf(stderr, "CMD[%s] with PID[%d] exited %s\n", job->cmd, job->data, successful ? "normally" : "abnormally");
                __handle_disp();
                delete(cpid, bg_procs);
            }
        }
    }
}

void sig_handler(int signum) {
    if (getpid() != master_shell_pid)
        return;

    if (signum == SIGINT) {
        if (curr_fg_pid == -1) {
            printf("\n");
            __handle_disp();
        }
    } else if (signum == SIGTSTP) {
        if (curr_fg_pid == -1) {
            printf("\n"); __handle_disp();
        }
    }

    return;
}
