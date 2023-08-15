#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>

#define MAX_COUNT 10

int count_shared = 0;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

void sigint_sig(int i){
    printf("Signal SIGINT detected. Counter of kit's: %d. Exiting...\n", count_shared);
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    key_t key_one;
    key_t key_two;
    int shmid;
    int *shared_memory;
    int semid;
    union semun arg;
    int count_of_kits = rand() % 10 + 1;
    int PID;
    int kit_size = 0;

    key_one = ftok("/tmp", 'R');
    if (key_one == -1){
        perror("Key creating error");
        exit(EXIT_FAILURE);
    }

    semid = semget(key_one, 1, 0666 | IPC_CREAT);
    if (semid == -1){
        perror("Semaphore get error");
        exit(EXIT_FAILURE);
    }

    arg.val = 0;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("Semaphore control error");
        exit(EXIT_FAILURE);
    }

    struct sembuf sem_lock_write = {0, 0, 0};
    struct sembuf sem_unlock_write = {0, 2, 0};
    struct sembuf sem_lock_print = {0, 0, 0};
    struct sembuf sem_lock_read = {0, -1, 0};
    struct sembuf sem_unlock_read = {0, -1, 0};

    key_two = ftok("/tmp", 'A');
    if (key_two == -1){
        perror("Key creating error");
        exit(EXIT_FAILURE);
    }

    shmid = shmget(key_two, sizeof(int) * (MAX_COUNT + 2), IPC_CREAT | 0666);
    if (shmid == -1){
        perror("Shared memory get error1");
        exit(EXIT_FAILURE);
    }

    shared_memory = shmat(shmid, NULL, 0);
    if (shared_memory == (void *)-1){
        perror("Shared memory at error2");
        exit(EXIT_FAILURE);
    }

    switch (PID = fork()) {
        case -1:{
            perror("Fork error");
            exit(EXIT_FAILURE);
        }
        case 0:{
            while (1){
                int min = 100;
                int max = 0;
                semop(semid, &sem_lock_read, 1);
                kit_size = shared_memory[0];
                if (kit_size != 0){
                    for (int i = 1; i < kit_size + 1; i++){
                        if (shared_memory[i] > max)
                            max = shared_memory[i];
                        if (shared_memory[i] < min)
                            min = shared_memory[i];
                    }
                } else{
                    min = 0;
                    max = 0;
                }
                shared_memory[MAX_COUNT + 1] = min;
                shared_memory[MAX_COUNT + 2] = max;
                semop(semid, &sem_unlock_read, 1);
            }
        }
        default:{
            signal(SIGINT, sigint_sig);
            srand(time(NULL));
            for (int i = 0; i < count_of_kits; i++){
                int rand_count = rand() % MAX_COUNT + 1;
                semop(semid, &sem_lock_write, 1);
                shared_memory[0] = rand_count;
                printf("Parent generated %d numbers: ", rand_count);
                for (int j = 1; j < rand_count + 1; j++){
                    int val = rand() % 100;
                    shared_memory[j] = val;
                    printf("%d ", shared_memory[j]);
                }
                printf("\n");
                semop(semid, &sem_unlock_write, 1);
                semop(semid, &sem_lock_print, 1);
                printf("Min: %d\n", shared_memory[MAX_COUNT + 1]);
                printf("Max: %d\n", shared_memory[MAX_COUNT + 2]);
                (count_shared)++;
                sleep(1);
            }
            shmdt(shared_memory);
        }
    }
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID, 0);
    raise(SIGINT);
    exit(EXIT_SUCCESS);
}