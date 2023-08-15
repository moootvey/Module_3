#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <semaphore.h>

int main() {
    int fd_fifo;
    int randNum;
    sem_t *semaphore;
    int i = 0;

    if ((semaphore = sem_open("/my_semaphore", O_CREAT, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    if ((fd_fifo = open("/tmp/fifo0005.1", O_WRONLY | O_CREAT | O_NONBLOCK, S_IRWXU)) == -1) {
        fprintf(stderr, "Невозможно открыть fifo\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    if (sem_wait(semaphore) == -1) {
        perror("Ошибка при блокировке семафора");
        exit(EXIT_FAILURE);
    }

    while(i != 10) {
        randNum = rand() % 100;

        write(fd_fifo, &randNum, sizeof(int));
        sleep(2);
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
    unlink("/tmp/fifo0005.1");

    if (sem_unlink("/my_semaphore") == -1) {
        perror("Ошибка при удалении семафора");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}