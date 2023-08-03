#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void signal_handler(int signal){
    if (signal == SIGINT){
        printf("Signal received SIGINT. End of the program...\n");
        exit(EXIT_SUCCESS);

    }
    else if (signal == SIGQUIT){
        printf("Signal received SIGQUIT. End of the program with a memory dump...\n");
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char *argv[]) {
    FILE* file;
    int i = 0;

    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

    while(1){
        file = fopen("text.txt", "a");
        i++;
        //printf("%d\n", i);
        fprintf(file, "%d ", i);
        sleep(1);
        fclose(file);
    }
    exit(EXIT_SUCCESS);
}
