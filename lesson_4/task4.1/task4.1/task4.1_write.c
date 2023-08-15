#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int main() {
    int fd_fifo;
    int randNum;
    int sem_id;
    int i = 0;
    key_t key;

    if ((key = ftok("/tmp", 1)) == -1)
    {
        perror("ftok semaphor:");
        exit(EXIT_FAILURE);
    }

    if ((sem_id = semget(key, 1, 0666 | IPC_CREAT)) == -1)
    {
        perror("semget sem:");
        exit(EXIT_FAILURE);
    }

    if ((fd_fifo = open("/tmp/fifo0005.1", O_WRONLY | O_CREAT | O_NONBLOCK, S_IRWXU)) == -1)
    {
        fprintf(stderr, "Невозможно открыть fifo\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    semctl(sem_id,0,SETVAL, 1);
    struct sembuf sem_lock = {0, -1, SEM_UNDO};
    struct sembuf sem_unlock = {0, 1, SEM_UNDO};

    if (semop(sem_id, &sem_lock, 1) == -1) {
        perror("Ошибка при блокировке семафора");
        exit(EXIT_FAILURE);
    }

    while(i != 10)
    {
        randNum = rand()%100;

        write(fd_fifo,&randNum,sizeof(int));
        sleep(1);
        printf("%d\n", randNum);
        i++;
    }

    if (semop(sem_id, &sem_unlock, 1) == -1) {
        perror("Ошибка при разблокировке семафора");
        exit(EXIT_FAILURE);
    }

    if (semop(sem_id, &sem_lock, 1) == -1) {
        perror("Ошибка при блокировке семафора");
        exit(EXIT_FAILURE);
    }
    close(fd_fifo);
    unlink("/tmp/fifo0005.1");
    if (semop(sem_id, &sem_unlock, 1) == -1) {
        perror("Ошибка при разблокировке семафора");
        exit(EXIT_FAILURE);
    }
    semctl(sem_id, 0, IPC_RMID);
    exit(EXIT_SUCCESS);
}