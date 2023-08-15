#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#define READERS_COUNT 3
#define MAX_FORK 5

typedef union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
} semun;

int main(int argc, char *argv[]) {
    int fd[2];
    key_t key;
    semun arg;
    int sem_id;

    if ((key = ftok("/tmp", 2)) == -1)
    {
        perror("Key creating error");
        exit(EXIT_FAILURE);
    }

    if ((sem_id = semget(key, 2, 0666 | IPC_CREAT)) == -1)
    {
        perror("Semget error");
        exit(EXIT_FAILURE);
    }
    arg.val = READERS_COUNT;
    semctl(sem_id,0,SETVAL, arg);
    arg.val = MAX_FORK;
    semctl(sem_id, 1, SETVAL, arg);

    struct sembuf sem_read_lock = {0, -1, SEM_UNDO};
    struct sembuf sem_read_unlock = {0, 1, SEM_UNDO};
    struct sembuf sem_write_lock = {1, -READERS_COUNT, SEM_UNDO};
    struct sembuf sem_write_unlock = {1, READERS_COUNT, SEM_UNDO};

    pid_t pid;

    if (pipe(fd) == -1) {
        perror("Pipe creation error.");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_FORK; i++){
        switch (pid = fork()) {
            case -1:{
                perror("Child creation error.");
                exit(EXIT_FAILURE);
            }
            case 0:{
                //semop(sem_id, &sem_lock_reader, 1);
                close(fd[0]);
                srand(getpid());

                int val;
                FILE *input_file;
                semop(sem_id, &sem_read_lock, 1);
                input_file = fopen("output.txt", "r");
                if (input_file == NULL) {
                    perror("File open error.");
                    exit(EXIT_FAILURE);
                }

                fscanf(input_file, "%d", &val);
                printf("Child read number: ");
                printf("%d\n", val);
                fclose(input_file);

                val = val * 2;
                write(fd[1], &val, sizeof(int));

                sleep(1);
                close(fd[1]);
                semop(sem_id, &sem_read_unlock, 1);
                break;
            }
            default:{
                sleep(3);
                close(fd[1]);
                FILE *output_file;
                int received_number;

                semop(sem_id, &sem_write_lock, 1);

                output_file = fopen("output.txt", "w");
                if (output_file == NULL) {
                    perror("File open error.");
                    exit(EXIT_FAILURE);
                }
                printf("Parent received numbers: ");
                while (read(fd[0], &received_number, sizeof(int)) > 0) {
                    printf("%d ", received_number);
                    fprintf(output_file, "%d ", received_number);
                }
                printf("\nNumbers written to file 'output.txt'\n");
                fclose(output_file);
                semop(sem_id, &sem_write_unlock, 1);
                close(fd[0]);
                break;
            }

        }
    }
    semctl(sem_id, 0, IPC_RMID);
    semctl(sem_id, 1, IPC_RMID);

    exit(EXIT_SUCCESS);
}