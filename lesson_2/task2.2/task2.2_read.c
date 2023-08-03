#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 1024

int main(int argc, char* argv[]){
    char str[MAX_LENGTH];
    FILE *file = fopen(argv[1], "r");
    if (file == NULL){
        perror("Cannot open file.\n");
        exit(EXIT_FAILURE);
    }
    fread(str, sizeof str[0], MAX_LENGTH, file);
    printf("%s", str);
    fclose(file);
    exit(EXIT_SUCCESS);
}