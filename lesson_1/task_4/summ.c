#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    int summ;
    for(int i=0; i!=argc; i++){
        int a = atoi(argv[i]);
        summ += a;
    }
    printf("Summ: %d\n", summ);
    exit(1);
}