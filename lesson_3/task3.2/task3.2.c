#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    FILE* file;
    int i = 0;
    int count = 0;

    signal(SIGINT, SIG_IGN);

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
