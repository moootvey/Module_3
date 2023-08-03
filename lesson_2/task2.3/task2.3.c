#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc, char* argv[]) {
    DIR *direct = opendir(argv[1]);
    struct dirent *entery;
    while((entery = readdir(direct)) != NULL){
        printf("%s\n", entery->d_name);
    }
    closedir(direct);
}
