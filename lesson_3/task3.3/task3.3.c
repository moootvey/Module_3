#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    FILE* file;
    int i = 0;
    int count = 0;

    while(1){
        if (signal(SIGINT, SIG_IGN) != NULL){
            if (count > 3)
                signal(SIGINT, SIG_DFL);
            count++;
        }

        file = fopen("text.txt", "a");
        i++;
        //printf("%d\n", i);
        fprintf(file, "%d ", i);
        sleep(1);
        fclose(file);
    }
    exit(EXIT_SUCCESS);
}
