#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_LENGHT 100

int main(int argc, char* argv[]) {
    int fd = open(argv[1], O_WRONLY | O_APPEND);
    char str[MAX_LENGHT];
    fgets(str, MAX_LENGHT,stdin);
    if (fd == -1){
        perror("Cannot open file.\n");
        exit(EXIT_FAILURE);
    }
    ssize_t bytes_written = write(fd, str, strlen(str));
    if (bytes_written == -1){
        perror("Unable to write string to file.\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
    exit(EXIT_SUCCESS);
}
