#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

void printFileType(const char *filename) {
    struct stat file_stat;

    if (stat(filename, &file_stat) == 0) {
        if (S_ISREG(file_stat.st_mode)) {
            printf("Regular file: %s\n", filename);
        } else if (S_ISDIR(file_stat.st_mode)) {
            printf("Directory: %s\n", filename);
        } else if (S_ISLNK(file_stat.st_mode)) {
            printf("Symbolic link: %s\n", filename);
        } else if (S_ISCHR(file_stat.st_mode)) {
            printf("Character device: %s\n", filename);
        } else if (S_ISBLK(file_stat.st_mode)) {
            printf("Block device: %s\n", filename);
        } else if (S_ISFIFO(file_stat.st_mode)) {
            printf("FIFO/pipe: %s\n", filename);
        } else if (S_ISSOCK(file_stat.st_mode)) {
            printf("Socket: %s\n", filename);
        } else {
            printf("Unknown: %s\n", filename);
        }
    } else {
        perror("stat");
    }
}

int main(int argc, char *argv[]) {

    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(argv[1])) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] != '.') {
                char full_path[1024];
                snprintf(full_path, sizeof(full_path), "%s/%s", argv[1], ent->d_name);
                printFileType(full_path);
            }
        }
        closedir(dir);
    } else {
        perror("opendir");
        return 1;
    }

    return 0;
}
