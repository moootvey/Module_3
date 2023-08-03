#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    int high_value = 0;
    int value = 0;
    for(int i = 0; i != argc; i++){
        value = atoi(argv[i]);
        if(high_value < value)
            high_value = value;
    }
    printf("Highest value: %d\n", high_value);
    exit(1);
}