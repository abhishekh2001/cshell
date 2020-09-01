#include <stdio.h>
#include <sys/utsname.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <malloc.h>

#define STR_SIZE 1000000
#define SPACE " "

// get rid of trailing newline char
void tokenize(char* line, char* res) {
   char* cmd = strtok(line," \t");
   char sp[] = " ";
   strcpy(res, cmd);

   while (cmd != NULL) {
        // printf ("token: %s\n",cmd);
        cmd = strtok(NULL, " \t");
        if (cmd) {
            size_t length = strlen(res);
            res[length] = ' ';
            res[length+1] = '\0';
            strcat(res, cmd);
        }
    } 
}

// end with newline
int echo_implementation(char* command) {
    printf("Init echo command: %s\n", command);
    char* line = (char*) malloc(sizeof(char) * strlen(command));
    strcpy(line, command);
    
    char* cmd = strtok(line," \t");
    cmd = strtok(NULL, " \t");

    while (cmd != NULL) {
        printf ("%s ", cmd);
        cmd = strtok(NULL, " \t");
    }
    printf("\n");
    return 0;
}

// ASSUMPTION: no spaces in path
int cd_implementation(char* line) {
    printf("Line %s\n", line);
   char* cmd = strtok(line, " \t");
   printf("init %s\n", cmd);
   cmd = strtok(NULL, " \t");
   printf("Next %s\n" ,cmd);
   if (cmd == NULL) {
       printf("cd error: no path specified\n");
       return -1;
   }
   if (strtok(NULL, " ") != NULL) {
       printf("cd error: too many arguments or space encountered\n");
       return -1;
   }

   if (chdir(cmd) < 0) {
       perror("Error chdir");
       return -1;
   }
   return 0;
}

int get_prompt(char* str) {
    struct utsname val;
    char username[STR_SIZE], path[STR_SIZE], temp_ch[2];
    temp_ch[1] = '\0';

    if (gethostname(username, STR_SIZE) < 0) {
        perror("Error retrieving hostname");
        return -1;
    }
    if (uname(&val) < 0) {
        perror("Failed to retrieve user details");
        return -1;
    }
    if (getcwd(path, STR_SIZE) == NULL) {
        perror("Error getting cwd");
        return -1;
    }
    if (str == 0) {
        return -1;
    }

    temp_ch[0] = '<';
    strcpy (str, temp_ch);
    strcat(str, username); (temp_ch[0] = '@');
    strcat(str, temp_ch);
    strcat(str, val.sysname); temp_ch[0] = ':';
    strcat(str, temp_ch);
    strcat(str, path); temp_ch[0] = '>';
    strcat(str, temp_ch);
}

int main() {
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
        tokenize(inp, clean_inp);
        printf("Clean input: %s-Done\n", clean_inp);

        // handle cd
        char* temp_clean_inp = malloc(STR_SIZE);
        strcpy(temp_clean_inp, clean_inp);
        if (strcmp(strtok(temp_clean_inp, " "), "cd") == 0) {
            cd_implementation(clean_inp);
        } else {
            echo_implementation(clean_inp);
        }
        fflush(stdin);
    }
}