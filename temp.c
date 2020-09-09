#include <unistd.h>
#include <stdio.h>

int main() {
    char line[256];
    printf("Enter: ");
    scanf("%s", line);
    printf("Entered: %s\n", line);
}
