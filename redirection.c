// Supposed to handle input/output file redirections
#include "redirection.h"
#include "main.h"

// TODO: Deal
// cat <what.txt
int handle_redirection() {
    file_in = -2;
    file_out = -2;
    if (is_out_redirect) {
            int flags = O_CREAT | O_WRONLY |
                ((is_append) ? O_APPEND : O_TRUNC);
            
            file_out = open(out_file_name, flags, 0644);

            if (file_out < 0) {
                perror("Error redirecting 'out'");
                return -1;
            }

            if ((stdout_copy = dup(STDOUT_FILENO)) < 0) {
                perror("Error redirecting 'out'");
                return -1;
            }

            if (dup2(file_out, STDOUT_FILENO) < 0) {
                perror("Error redirecting 'out'");
                return -1;
            }
        }

        if (is_in_redirect) {
            int flags = O_RDONLY;
            
            file_in = open(in_file_name, flags);

            if (file_in < 0) {
                perror("Error redirecting 'in'");
                return -1;
            }

            if ((stdin_copy = dup(STDIN_FILENO)) < 0) {
                perror("Error redirecting 'in'");
                return -1;
            }

            if (dup2(file_in, STDIN_FILENO) < 0) {
                perror("Error redirecting 'in'");
                return -1;
            }
        }
}

void clean_redirection() {
    if (file_in >= 0) {
        if (dup2(stdin_copy, STDIN_FILENO) < 0) {
            perror("Error swapping STDIN_FILENO file");
        }
        if (close(file_in) < 0) {
            perror("Error closing file_in");
        }
        if (close(stdin_copy) < 0) {
            perror("Error closing stdin_copy");
        }
    }

    if (file_out >= 0) {  // We have a file open for STDOUT
        if (dup2(stdout_copy, STDOUT_FILENO) < 0) {
            perror("Error swapping STDOUT_FILENO file");
        }
        if (close(file_out) < 0) {
            perror("Error closing file_out");
        }
        if (close(stdout_copy) < 0) {
            perror("Error closing stdout_copy");
        }
    }
}