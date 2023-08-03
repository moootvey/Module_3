#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define STR_SIZE 1024

int main(int argc, char*argv[]){
    int fd = open(argv[1], O_RDONLY);
    char str[STR_SIZE];
    ssize_t bytes_read;
    char line[STR_SIZE];
    size_t line_length = 0;

    if (fd == -1){
        perror("Cannot open file.\n");
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = read(fd, str, sizeof(str))) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            if (str[i] != '\n') {
                if (line_length < sizeof(line) - 1) {
                    line[line_length++] = str[i];
                }
            } else {
                line[line_length] = '\0';
                printf("%s\n", line);
                line_length = 0;
            }
        }
    }

    if (line_length > 0) {
        line[line_length] = '\0';
        printf("%s\n", line);
    }
    close(fd);
    exit(EXIT_SUCCESS);
}