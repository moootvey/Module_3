#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_LENGTH 1024
#define MAX_ARGS 64

int main() {
    char input[MAX_INPUT_LENGTH];
    char* args[MAX_ARGS];
    int status;

    while (1) {
        printf(">> ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Input error.\n");
            continue;
        }

        input[strcspn(input, "\n")] = '\0';

        char* token;
        int arg_count = 0;

        token = strtok(input, " ");
        while (token != NULL && arg_count < MAX_ARGS - 1) {
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_count] = NULL;

        if (strcmp(args[0], "exit") == 0) {
            printf("Exitting...\n");
            break;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork error.\n");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {

            execvp(args[0], args);

            perror("Boot prog error.\n");
            exit(EXIT_FAILURE);
        } else {
            waitpid(pid, &status, 0);

            if (WIFEXITED(status)) {
                printf("Program exited with code: %d\n", WEXITSTATUS(status));
            } else {
                printf("The program ended incorrectly.\n");
            }
        }
    }
    exit(EXIT_SUCCESS);
}