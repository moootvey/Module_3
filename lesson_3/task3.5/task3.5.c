#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static FILE* file;

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
    int i = 0;

    while(1){
        file = fopen("text.txt", "a");
        sleep(1);
        signal(SIGINT, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);
        if (file != NULL){
            i++;
            fprintf(file, "%d ", i);
            fclose(file);
        }
        signal(SIGINT, signal_handler);
        signal(SIGQUIT, signal_handler);
    }
    exit(EXIT_SUCCESS);
}
