#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 100

int main(int argc, char* argv[]) {
    char str[MAX_LENGTH];
    FILE *file = fopen(argv[1], "a");
    if (file == NULL){
        perror("Cannot open file.\n");
        exit(EXIT_FAILURE);
    }
    if (fgets(str, MAX_LENGTH, stdin) != NULL) {
        size_t length = strcspn(str, "\n");

        if (length < MAX_LENGTH - 1) {
            str[length] = '\n';
            str[length + 1] = '\0';
        }
        fwrite(str, sizeof(str[0]), strlen(str), file);
    }
    fclose(file);
    exit(EXIT_SUCCESS);
}
