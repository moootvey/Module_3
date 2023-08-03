#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

#define MAX_LENGTH 100

int fileExists(char* argv[]) {
    DIR* dir = opendir(argv[2]);
    if (dir == NULL) {
        perror("Error opening directory.\n");
        exit(EXIT_FAILURE);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, argv[1]) == 0) {
            closedir(dir);
            return 1; // Файл найден
        }
    }

    closedir(dir);
    return 0; // Файл не найден
}

int main(int argc, char* argv[]) {
    char str[MAX_LENGTH];
    FILE* file;
    char FileName[MAX_LENGTH];
    strcpy(FileName, argv[2]);
    strcat(FileName, argv[1]);

    if (fileExists(argv)) {
        file = fopen(FileName, "a");
        if (file == NULL) {
            perror("Cannot open file.\n");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("File %s does not exist.\n", argv[1]);
        printf("Do you want to create it? (y/n): ");

        char response;
        response = getchar();

        if (response == 'y' || response == 'Y') {
            file = fopen(FileName, "a");
            if (file == NULL) {
                perror("Cannot create file.\n");
                exit(EXIT_FAILURE);
            }
        } else {
            printf("Exiting without creating the file.\n");
            exit(EXIT_FAILURE);
        }
    }
    fflush(stdin);
    fgets(str, MAX_LENGTH, stdin);
    if (str != NULL) {
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
