#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <semaphore.h>

int main(int argc, char* argv[]) {
    int fd_fifo;
    int randNum;
    sem_t *semaphore;
    int i = 0;

    if ((semaphore = sem_open("/my_semaphore", O_CREAT, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    if ((fd_fifo = open("/tmp/fifo0005.1", O_RDONLY | O_NONBLOCK)) == -1) {
        fprintf(stderr, "Невозможно открыть fifo\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    if (sem_wait(semaphore) == -1) {
        perror("Ошибка при блокировке семафора");
        exit(EXIT_FAILURE);
    }

    while (i != 10) {
        read(fd_fifo, &randNum, sizeof(int));
        printf("%d\n", randNum);
        i++;
    }

    if (sem_post(semaphore) == -1) {
        perror("Ошибка при разблокировке семафора");
        exit(EXIT_FAILURE);
    }

    if (sem_close(semaphore) == -1) {
        perror("Ошибка при закрытии семафора");
        exit(EXIT_FAILURE);
    }

    close(fd_fifo);
    exit(EXIT_SUCCESS);
}