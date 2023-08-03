#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int fd1[2];
    int fd2[2];
    pid_t pid;

    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("Pipe creation error.");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid < 0) {
        perror("Child creation error.");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        close(fd1[1]);
        close(fd2[0]);

        FILE *output_file = fopen("output.txt", "a");
        if (output_file == NULL) {
            perror("File open error.");
            exit(EXIT_FAILURE);
        }

        int received_number;
        int res;
        printf("Parent received numbers: ");
        fprintf(output_file, "Parent received numbers: ");
        while (read(fd1[0], &received_number, sizeof(int)) > 0) {
            printf("%d ", received_number);
            fprintf(output_file, "%d ", received_number);
            res = received_number * 2;
            write(fd2[1], &res, sizeof(int));
        }

        printf("\nNumbers written to file 'output.txt'\n");

        fclose(output_file);
        close(fd1[0]);
        close(fd2[1]);

        wait(NULL);
    } else {
        close(fd1[0]);
        close(fd2[1]);
        srand(getpid());

        int i, random_number;
        printf("Child generate numbers: ");
        int count = atoi(argv[1]);
        for (i = 0; i < count; ++i) {
            random_number = rand() % 100;
            printf("%d ", random_number);
            write(fd1[1], &random_number, sizeof(int));
        }

        printf("\n");
        close(fd1[1]);
        FILE *output_file = fopen("output.txt", "a");
        if (output_file == NULL) {
            perror("File open error.");
            exit(EXIT_FAILURE);
        }
        int received_number_2;
        printf("Child received numbers: ");
        fprintf(output_file, "\nChild received numbers: ");
        while (read(fd2[0], &received_number_2, sizeof(int)) > 0){
            printf("%d ", received_number_2);
            fprintf(output_file, "%d ", received_number_2);
        }
        fprintf(output_file, "\n");
        printf("\nNumbers written to file 'output.txt'\n");
        fclose(output_file);
        close(fd2[0]);
    }
    exit(EXIT_SUCCESS);
}