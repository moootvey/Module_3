#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>

#define READERS_COUNT 3
#define MAX_FORK 5

int main() {
    int fd[2];
    sem_t *sem_rw, *sem_fork;
    pid_t pid;

    sem_unlink("/read_semaphore");
    sem_unlink("/write_semaphore");

    // Create the POSIX named semaphores
    sem_rw = sem_open("/read_semaphore", O_CREAT | O_EXCL, 0666, READERS_COUNT);
    if (sem_rw == SEM_FAILED) {
        perror("Failed to create read_semaphore");
        exit(EXIT_FAILURE);
    }

    sem_fork = sem_open("/write_semaphore", O_CREAT | O_EXCL, 0666, MAX_FORK);
    if (sem_fork == SEM_FAILED) {
        perror("Failed to create write_semaphore");
        sem_unlink("/read_semaphore");
        exit(EXIT_FAILURE);
    }

    if (pipe(fd) == -1) {
        perror("Pipe creation error.");
        sem_unlink("/read_semaphore");
        sem_unlink("/write_semaphore");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_FORK; i++) {
        switch (pid = fork()) {
            case -1:{
                perror("Child creation error.");
                sem_unlink("/read_semaphore");
                sem_unlink("/write_semaphore");
                exit(EXIT_FAILURE);
            }
            case 0:{
                sem_wait(sem_fork);
                close(fd[0]);
                srand(getpid());

                int val;
                FILE *input_file;
                sem_wait(sem_rw); // Reader lock
                input_file = fopen("output.txt", "r");
                if (input_file == NULL) {
                    perror("File open error.");
                    sem_post(sem_rw); // Release the lock before exiting
                    exit(EXIT_FAILURE);
                }

                fscanf(input_file, "%d", &val);
                printf("Child read number: %d\n", val);
                fclose(input_file);

                val = val * 2;
                write(fd[1], &val, sizeof(int));

                sleep(1);
                close(fd[1]);
                sem_post(sem_rw); // Reader unlock
                sem_post(sem_fork);
                break;
            }
            default:{
                sleep(3);
                close(fd[1]);
                FILE *output_file;
                int received_number;

                sem_wait(sem_rw); // Writer lock

                output_file = fopen("output.txt", "w");
                if (output_file == NULL) {
                    perror("File open error.");
                    sem_post(sem_rw); // Release the lock before exiting
                    sem_unlink("/read_semaphore");
                    sem_unlink("/write_semaphore");
                    exit(EXIT_FAILURE);
                }

                printf("Parent received numbers: ");
                while (read(fd[0], &received_number, sizeof(int)) > 0) {
                    printf("%d ", received_number);
                    fprintf(output_file, "%d ", received_number);
                }
                printf("\nNumbers written to file 'output.txt'\n");
                fclose(output_file);
                sem_post(sem_rw); // Writer unlock
                close(fd[0]);
                break;
            }
        }
    }

    sem_close(sem_rw);
    sem_close(sem_fork);
    sem_unlink("/read_semaphore");
    sem_unlink("/write_semaphore");

    for (int i = 0; i < MAX_FORK; i++) {
        wait(NULL);
    }

    exit(EXIT_SUCCESS);
}