#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>

#define MAX_COUNT 11
#define THREADS_COUNT 3

int count_shared = 0;

void sigint_sig(int i) {
    printf("Signal SIGINT detected. Counter of kit's: %d. Exiting...\n", count_shared);
    exit(EXIT_SUCCESS);
}

void* func_min(void* arg) {
    int min = 100;
    int kit_count = ((int*)arg)[0];
    if (kit_count != 0) {
        for (int i = 1; i < kit_count + 1; i++) {
            if (((int*)arg)[i] < min)
                min = ((int*)arg)[i];
        }
    } else {
        min = 0;
    }
    ((int*)arg)[MAX_COUNT] = min;
    pthread_exit(0);
}

void* func_summ(void* arg) {
    int summ = 0;
    int kit_count = ((int*)arg)[0];
    if (kit_count != 0) {
        for (int i = 1; i < kit_count + 1; i++) {
            summ += ((int*)arg)[i];
        }
    } else {
        summ = 0;
    }
    ((int*)arg)[MAX_COUNT + 1] = summ;
    pthread_exit(0);
}

void* func_mean(void* arg) {
    int mean = 0;
    int summ = 0;
    int kit_count = ((int*)arg)[0];
    if (kit_count != 0) {
        for (int i = 1; i < kit_count + 1; i++) {
            summ += ((int*)arg)[i];
        }
    } else {
        summ = 0;
    }
    mean = summ / kit_count;
    ((int*)arg)[MAX_COUNT + 2] = mean;
    pthread_exit(0);
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    const char* shmid = "/tmp_shmid";
    const char* semid = "/tmp_semid";
    int fd_shm;
    int *shared_memory;
    sem_t* semaphore;
    int count_of_kits = rand() % 10 + 1;
    int PID;

    fd_shm = shm_open(shmid, O_CREAT | O_RDWR, 0666);
    if (fd_shm == -1) {
        perror("Shared memory open error");
        exit(EXIT_FAILURE);
    }
    ftruncate(fd_shm, sizeof(int) * (MAX_COUNT + 2));

    shared_memory = (int*)mmap(NULL, sizeof(int) * (MAX_COUNT + 2), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    if (shared_memory == MAP_FAILED) {
        perror("Shared memory mapping error");
        exit(EXIT_FAILURE);
    }

    semaphore = sem_open(semid, O_CREAT, 0666, 1);
    if (semaphore == SEM_FAILED) {
        perror("Semaphore open error");
        exit(EXIT_FAILURE);
    }

    switch (PID = fork()) {
        case -1: {
            perror("Fork error");
            exit(EXIT_FAILURE);
        }
        case 0: {
            pthread_t tid[THREADS_COUNT];
            void* status[THREADS_COUNT];
            while (1) {
                sem_wait(semaphore);
                pthread_create(&tid[0], NULL, func_min, shared_memory);
                pthread_create(&tid[1], NULL, func_summ, shared_memory);
                pthread_create(&tid[2], NULL, func_mean, shared_memory);
                for (int i = 0; i < THREADS_COUNT; i++) {
                    pthread_join(tid[i], &status[i]);
                }
                sem_post(semaphore);
            }
        }
        default: {
            signal(SIGINT, sigint_sig);
            srand(time(NULL));
            for (int i = 0; i < count_of_kits; i++) {
                int rand_count = rand() % (MAX_COUNT - 1) + 1;
                sem_wait(semaphore);
                shared_memory[0] = rand_count;
                printf("Parent generated %d numbers: ", rand_count);
                for (int j = 1; j < rand_count + 1; j++) {
                    int val = rand() % 100;
                    shared_memory[j] = val;
                    printf("%d ", shared_memory[j]);
                }
                printf("\n");
                sem_post(semaphore);
                sleep(1);
                printf("Min: %d\n", shared_memory[MAX_COUNT]);
                printf("Summ: %d\n", shared_memory[MAX_COUNT + 1]);
                printf("Mean: %d\n", shared_memory[MAX_COUNT + 2]);
                (count_shared)++;
                sleep(1);
            }
            munmap(shared_memory, sizeof(int) * (MAX_COUNT + 2));
            close(fd_shm);
            shm_unlink(shmid);
            sem_close(semaphore);
            sem_unlink(semid);
            raise(SIGINT);
            exit(EXIT_SUCCESS);
        }
    }
}
