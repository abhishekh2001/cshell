#include <unistd.h>
#include <stdio.h>

int main() {
    FILE* f = fopen("/home/abhishekh/Documents/Assignments/OSN/shell-assignment/.history.txt", "r");
    if (f) {
        char line[256];
        while (fgets(line, sizeof(line), f))
                printf("%s\n", line);

    }
    fclose(f);
}
