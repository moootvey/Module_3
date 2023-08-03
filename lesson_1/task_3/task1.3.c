#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

void exit_status(){
    printf("Procces was finished!\n");
}

int main(int argc, char* argv[]) {
    pid_t pid;
    int rv;
    atexit(exit_status);
    switch (pid = fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            for(int i = (argc+1)/2; i != argc; i++){
                //sleep(4);
                int side = atoi(argv[i]);
                int square = side * side;
                printf("Square area (child) = %d\n", square);
            }
            exit(EXIT_SUCCESS);
        default:
            for(int i = 1; i != (argc+1)/2; i++){
                //sleep(4);
                int side = atoi(argv[i]);
                int square = side * side;
                printf("Square area (parent) = %d\n", square);
            }
            wait(&rv);
            printf("%d\n", WEXITSTATUS(rv));
    }
    exit(EXIT_SUCCESS);
}