#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void exit_status(){
    printf("All good!\n");
}

int main(int argc, char *argv[]) {
    pid_t pid;
    int rv;
    switch(pid=fork()){
        case -1:
        exit(-1);
        case 0:
            printf("%s\n", argv[2]);
            exit(1);
        default:
            printf("%s\n", argv[1]);
            wait(&rv);
            printf("%d\n", WEXITSTATUS(rv));
    }
    atexit(exit_status);
}
