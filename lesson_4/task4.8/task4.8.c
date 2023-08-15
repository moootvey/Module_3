#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <pthread.h>

#define MAX_COUNT 11
#define THREADS_COUNT 3

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

void* func_min(void* arg){
    int min = 100;
    int kit_count = ((int*)arg)[0];
    if (kit_count != 0){
        for (int i = 1; i < kit_count + 1; i++){
            if (((int*)arg)[i] < min)
                min = ((int*)arg)[i];
        }
    }
    else{
        min = 0;
    }
    ((int*)arg)[MAX_COUNT] = min;
    pthread_exit(0);
}

void* func_summ(void* arg){
    int summ = 0;
    int kit_count = ((int*)arg)[0];
    if (kit_count != 0){
        for (int i = 1; i < kit_count + 1; i++){
            summ += ((int*)arg)[i];
        }
    }
    else{
        summ = 0;
    }
    ((int*)arg)[MAX_COUNT + 1] = summ;
    pthread_exit(0);
}

void* func_mean(void* arg){
    int mean = 0;
    int summ = 0;
    int kit_count = ((int*)arg)[0];
    if (kit_count != 0){
        for (int i = 1; i < kit_count + 1; i++){
            summ += ((int*)arg)[i];
        }
    }
    else {
        summ = 0;
    }
    mean = summ/kit_count;
    ((int*)arg)[MAX_COUNT + 2] = mean;
    pthread_exit(0);
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
            pthread_t tid[THREADS_COUNT];
            void* status[THREADS_COUNT];
            while (1){
                semop(semid, &sem_lock_read, 1);
                pthread_create(&tid[0], NULL, func_min, shared_memory);
                pthread_create(&tid[1], NULL, func_summ, shared_memory);
                pthread_create(&tid[2], NULL, func_mean, shared_memory);
                for (int i = 0; i < THREADS_COUNT; i++){
                    pthread_join(tid[i], &status[i]);
                }
                semop(semid, &sem_unlock_read, 1);
            }
        }
        default:{
            signal(SIGINT, sigint_sig);
            srand(time(NULL));
            for (int i = 0; i < count_of_kits; i++){
                int rand_count = rand() % (MAX_COUNT - 1) + 1;
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
                printf("Min: %d\n", shared_memory[MAX_COUNT]);
                printf("Summ: %d\n", shared_memory[MAX_COUNT + 1]);
                printf("Mean: %d\n", shared_memory[MAX_COUNT + 2]);
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