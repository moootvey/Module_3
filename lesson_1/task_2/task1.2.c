#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void exit_status(){
    printf("Procces was finished\n");
}

void print_args(int argc, char* argv[]){
    for(int i=1; i!=argc; i++){
        printf("%s ", argv[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    pid_t pid;
    int rv;
    atexit(exit_status);
    switch(pid=fork()){
        case -1:
        exit(EXIT_FAILURE);
        case 0:
            printf("Child args: ");
            print_args(argc, argv);
            exit(EXIT_SUCCESS);
        default:
            printf("Parent args: ");
            print_args(argc, argv);
            wait(&rv);
            printf("%d\n", WEXITSTATUS(rv));
    }
    exit(EXIT_SUCCESS);
}
