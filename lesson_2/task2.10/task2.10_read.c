#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int fd_fifo; /*дескриптор FIFO*/
    int randNum;

    /*Открываем fifo для чтения*/
    if ((fd_fifo = open("/tmp/fifo0003.1", O_RDONLY)) == -1)
    {
        fprintf(stderr, "Невозможно открыть fifo\n");
        exit(EXIT_FAILURE);
    }
    srand(time(NULL));
    while (1)
    {
        read(fd_fifo, &randNum, sizeof(int));
        printf("%d\n", randNum);
    }
    close(fd_fifo);
    exit(EXIT_SUCCESS);
}