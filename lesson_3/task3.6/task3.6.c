#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int is_parent_modifying = 0;

void sigusr1_handler(int signum) {
    is_parent_modifying = 1;
    printf("File blocked for read.\n");
}

void sigusr2_handler(int signum) {
    is_parent_modifying = 0;
    printf("File unblocked for read.\n");
}

int main(int argc, char *argv[]) {
    int fd[2];
    pid_t pid;
    signal(SIGUSR1, sigusr1_handler);
    signal(SIGUSR2, sigusr2_handler);

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
        sleep(3);
        close(fd[1]);
        FILE *output_file;

        output_file = fopen("output.txt", "w");
        if (output_file == NULL) {
            perror("File open error.");
            exit(EXIT_FAILURE);
        }

        int received_number;
        raise(SIGUSR1);
        printf("Parent received numbers: ");
        while (read(fd[0], &received_number, sizeof(int)) > 0) {
            printf("%d ", received_number);
            fprintf(output_file, "%d ", received_number);
        }
        printf("\nNumbers written to file 'output.txt'\n");

        fclose(output_file);
        raise(SIGUSR2);

        close(fd[0]);
        wait(NULL);
    } else {
        close(fd[0]);
        srand(getpid());

        while (1) {
            if (is_parent_modifying == 1)
                pause();
            else {
                break;
            }
        }

        int val;
        FILE *input_file;
        input_file = fopen("output.txt", "r");
        if (input_file == NULL) {
            perror("File open error.");
            exit(EXIT_FAILURE);
        }

        fscanf(input_file, "%d", &val);
        printf("Child read number: ");
        printf("%d\n", val);
        fclose(input_file);

        val = val*2;
        write(fd[1], &val, sizeof(int));

        sleep(1);
        close(fd[1]);
    }

    return 0;
}