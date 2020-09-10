#include "main.h"
#include "history.h"

void open_history() {
    hist_ptr = 0;
    enable_history = 1;
    load_history();
}

void load_history() {
    FILE* hist_file = fopen(history_path, "r");
    if (!hist_file) {
        perror("Unable to load history file");
        return;
    }
    char line[HIST_STR_SIZE];
    while (fgets(line, sizeof(line), hist_file)) {
        line[strlen(line)-1] = '\0';
        strcpy(history[hist_ptr++], line);
    }
    fclose(hist_file);
}

void insert_history(char* cmd) {
    if (hist_ptr > 0 && strcmp(history[hist_ptr-1], cmd) == 0)
        return;
    if (hist_ptr == 20) {
        for (int i = 0; i < hist_ptr-1; i++) {
            strcpy(history[i], history[i+1]);
        }
        strcpy(history[hist_ptr-1], cmd);
    } else {
        strcpy(history[hist_ptr], cmd);
        hist_ptr++;
    }
}

void store_history() {
    FILE* hist_file = fopen(history_path, "w");
    if (!hist_file)
        return;
    for (int i = 0; i < hist_ptr; i++) {
        fprintf(hist_file, "%s\n", history[i]);
    }
    fclose(hist_file);
}

void disp_history(int num) {  /* will not allow for preservation if history file cannot be accessed */
    for (int i = max(0, hist_ptr-num); i < hist_ptr; i++) {
        printf("%s\n", history[i]);
    }
}

void cleanup_history() {
    store_history();
}