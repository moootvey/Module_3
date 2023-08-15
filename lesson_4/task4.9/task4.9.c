#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>

#define MAX_COUNT 10

int count_shared = 0;

void sigint_sig(int i){
    printf("Signal SIGINT detected. Counter of kit's: %d. Exiting...\n", count_shared);
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    const char* sem_name = "/tmp_sem";
    const char* shm_name = "/tmp_shm";
    sem_t *semaphore;
    int *shared_memory;
    int count_of_kits = rand() % 10 + 1;
    int kit_size = 0;

    semaphore = sem_open(sem_name, O_CREAT, 0666, 1);
    if (semaphore == SEM_FAILED){
        perror("Semaphore creation error");
        exit(EXIT_FAILURE);
    }

    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1){
        perror("Shared memory creation error");
        exit(EXIT_FAILURE);
    }
    ftruncate(shm_fd, sizeof(int) * (MAX_COUNT + 2));

    shared_memory = mmap(NULL, sizeof(int) * (MAX_COUNT + 2), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED){
        perror("Shared memory mapping error");
        exit(EXIT_FAILURE);
    }

    switch (fork()) {
        case -1:{
            perror("Fork error");
            exit(EXIT_FAILURE);
        }
        case 0:{
            while (1){
                int min = 100;
                int max = 0;
                sem_wait(semaphore);
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
                sem_post(semaphore);
            }
        }
        default:{
            signal(SIGINT, sigint_sig);
            srand(time(NULL));
            for (int i = 0; i < count_of_kits; i++){
                int rand_count = rand() % MAX_COUNT + 1;
                sem_wait(semaphore);
                shared_memory[0] = rand_count;
                printf("Parent generated %d numbers: ", rand_count);
                for (int j = 1; j < rand_count + 1; j++){
                    int val = rand() % 100;
                    shared_memory[j] = val;
                    printf("%d ", shared_memory[j]);
                }
                printf("\n");
                sem_post(semaphore);
                sleep(1);
                printf("Min: %d\n", shared_memory[MAX_COUNT + 1]);
                printf("Max: %d\n", shared_memory[MAX_COUNT + 2]);
                (count_shared)++;
                sleep(1);
            }
            munmap(shared_memory, sizeof(int) * (MAX_COUNT + 2));
            close(shm_fd);
            shm_unlink(shm_name);
            sem_close(semaphore);
            sem_unlink(sem_name);
        }
    }

    raise(SIGINT);
    exit(EXIT_SUCCESS);
}