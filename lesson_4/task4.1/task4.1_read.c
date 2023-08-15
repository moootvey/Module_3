#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int main(int argc, char* argv[]) {
    int fd_fifo;
    int randNum;
    key_t key;
    int i = 0;

    if ((key = ftok("/tmp", 1)) == -1)
    {
        perror("ftok semaphor:");
        exit(EXIT_FAILURE);
    }

    int sem_id = semget(key, 1, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("Ошибка при создании/получении семафора");
        exit(EXIT_FAILURE);
    }

    struct sembuf sem_lock = {0, -1, SEM_UNDO};
    struct sembuf sem_unlock = {0, 1, SEM_UNDO};

    if ((fd_fifo = open("/tmp/fifo0005.1", O_RDONLY | O_NONBLOCK)) == -1)
    {
        fprintf(stderr, "Невозможно открыть fifo\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    if (semop(sem_id, &sem_lock, 1) == -1) {
        perror("Ошибка при блокировке семафора");
        exit(EXIT_FAILURE);
    }

    while (i != 10)
    {
        read(fd_fifo, &randNum, sizeof(int));
        printf("%d\n", randNum);
        i++;
    }

    if (semop(sem_id, &sem_unlock, 1) == -1) {
        perror("Ошибка при разблокировке семафора");
        exit(EXIT_FAILURE);
    }

    close(fd_fifo);
    exit(EXIT_SUCCESS);
}