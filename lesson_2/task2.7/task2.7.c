#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int fd[2];
    pid_t pid;

    if (pipe(fd) == -1) {
        perror("Pipe creation error.");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid < 0) {
        perror("Child creation error.");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        close(fd[1]);

        FILE *output_file = fopen("output.txt", "w");
        if (output_file == NULL) {
            perror("File open error.");
            exit(EXIT_FAILURE);
        }

        int received_number;
        printf("Parent received numbers: ");
        fprintf(output_file, "Parent received numbers: ");
        while (read(fd[0], &received_number, sizeof(int)) > 0) {
            printf("%d ", received_number);
            fprintf(output_file, "%d ", received_number);
        }

        printf("\nNumbers written to file 'output.txt'\n");

        fclose(output_file);
        close(fd[0]);
        wait(NULL);
    } else {
        close(fd[0]);

        srand(getpid());

        int i, random_number;
        printf("Child generate numbers: ");
        int count = atoi(argv[1]);
        for (i = 0; i < count; ++i) {
            random_number = rand() % 100;
            printf("%d ", random_number);
            write(fd[1], &random_number, sizeof(int));
        }

        printf("\n");
        close(fd[1]);
    }

    return 0;
}