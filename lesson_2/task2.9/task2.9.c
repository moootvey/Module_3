#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_LENGTH 1024
#define MAX_ARGS 64

char** splitCommands(const char* command, int* numCommands) {
    // Определяем максимальное количество разделенных команд (пусть будет 10)
    int maxCommands = 10;
    char** commands = (char**)malloc(maxCommands * sizeof(char*));
    *numCommands = 0;

    // Копируем строку команды, чтобы избежать изменения исходной строки
    char* commandCopy = strdup(command);

    // Разделяем команды по символу '|'
    char* token = strtok(commandCopy, "|");
    while (token != NULL) {
        // Удаляем начальные и конечные пробелы у команды
        while (*token == ' ') {
            token++;
        }
        size_t len = strlen(token);
        while (len > 0 && token[len - 1] == ' ') {
            token[--len] = '\0';
        }

        // Копируем команду в массив
        commands[(*numCommands)++] = strdup(token);

        // Проверяем, необходимо ли увеличить размер массива команд
        if (*numCommands >= maxCommands) {
            maxCommands *= 2;
            commands = (char**)realloc(commands, maxCommands * sizeof(char*));
        }

        // Переходим к следующей команде
        token = strtok(NULL, "|");
    }

    free(commandCopy);
    return commands;
}

int main() {
    char input[MAX_INPUT_LENGTH];
    int status;
    int numCommands;
    int rv;

    while (1) {
        char* args[MAX_ARGS];
        printf(">> ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Input error.\n");
            continue;
        }

        input[strcspn(input, "\n")] = '\0';

        char* token;
        int arg_count = 0;

        char** commands = splitCommands(input, &numCommands);
        switch(numCommands){
            case 1:{
                arg_count = 0;
                char* argv[MAX_ARGS];
                argv[0] = commands[0];
                token = strtok(argv[0], " ");
                while (token != NULL && arg_count < MAX_ARGS - 1) {
                    args[arg_count++] = token;
                    token = strtok(NULL, " ");
                }
                args[arg_count] = NULL;

                if (strcmp(args[0], "exit") == 0) {
                    printf("Exitting...\n");
                    exit(EXIT_SUCCESS);
                }
                switch (fork()) {
                    case -1:
                        perror("Fork error!");
                        exit(EXIT_FAILURE);
                    case 0:
                        execvp(args[0], args);
                    default:
                        wait(&rv);
                        break;
                }
                arg_count = 0;
                free(args[0]);
                free(commands);
                break;
            }
            case 2:{
                arg_count = 0;
                int fd[2];
                pipe(fd);
                char* argv[MAX_ARGS];

                argv[0] = commands[0];
                argv[1] = commands[1];

                token = strtok(argv[0], " ");
                while (token != NULL && arg_count < MAX_ARGS - 1) {
                    args[arg_count++] = token;
                    token = strtok(NULL, " ");
                }
                args[arg_count] = NULL;

                if (strcmp(args[0], "exit") == 0) {
                    printf("Exitting...\n");
                    break;
                }

                switch(fork()){
                    case -1:
                        perror("Fork error!");
                        exit(EXIT_FAILURE);
                    case 0:
                        close(fd[0]);
                        dup2(fd[1], STDOUT_FILENO);
                        execvp(args[0], args);
                    default:
                        break;
                }
                close(fd[1]);
                arg_count = 0;
                char* args2[MAX_ARGS];
                token = strtok(argv[1], " ");
                while (token != NULL && arg_count < MAX_ARGS - 1) {
                    args2[arg_count++] = token;
                    token = strtok(NULL, " ");
                }
                args2[arg_count] = NULL;

                switch (fork()) {
                    case -1:
                        perror("Fork error!");
                        exit(EXIT_FAILURE);
                    case 0:
                        close(fd[1]);
                        dup2(fd[0], STDIN_FILENO);
                        execvp(args2[0], args2);
                }
                arg_count = 0;
                for (int i = 0; i < numCommands; i++)
                    free(argv[i]);
                free(commands);
                break;
            }
            case 3:{
                arg_count = 0;
                int fd[2];
                int fd_2[2];
                pipe(fd);
                pipe(fd_2);
                char* argv[MAX_ARGS];

                argv[0] = commands[0];
                argv[1] = commands[1];
                argv[2] = commands[2];

                token = strtok(argv[0], " ");
                while (token != NULL && arg_count < MAX_ARGS - 1) {
                    args[arg_count++] = token;
                    token = strtok(NULL, " ");
                }
                args[arg_count] = NULL;

                if (strcmp(args[0], "exit") == 0) {
                    printf("Exitting...\n");
                    break;
                }
                switch(fork()){
                    case -1:
                        perror("Fork error!");
                        exit(EXIT_FAILURE);
                    case 0:
                        close(fd[0]);
                        dup2(fd[1], STDOUT_FILENO);
                        execvp(args[0], args);
                    default:
                        break;
                }
                close(fd[1]);
                arg_count = 0;
                char* args2[MAX_ARGS];
                token = strtok(argv[1], " ");
                while (token != NULL && arg_count < MAX_ARGS - 1) {
                    args2[arg_count++] = token;
                    token = strtok(NULL, " ");
                }
                args2[arg_count] = NULL;

                switch (fork()) {
                    case -1:
                        perror("Fork error!");
                        exit(EXIT_FAILURE);
                    case 0:
                        close(fd[1]);
                        close(fd_2[0]);
                        dup2(fd[0], STDIN_FILENO);
                        dup2(fd_2[1], STDOUT_FILENO);
                        execvp(args2[0], args2);
                }

                arg_count = 0;
                char* args3[MAX_ARGS];
                token = strtok(argv[2], " ");
                while (token != NULL && arg_count < MAX_ARGS - 1) {
                    args3[arg_count++] = token;
                    token = strtok(NULL, " ");
                }
                args3[arg_count] = NULL;

                switch (fork()) {
                    case -1:
                        perror("Fork error!");
                        exit(EXIT_FAILURE);
                    case 0:
                        close(fd[0]);
                        close(fd[1]);
                        close(fd_2[1]);
                        dup2(fd_2[0], STDIN_FILENO);
                        //dup2(fd_2[0], STDIN_FILENO);
                        execvp(args3[0], args3);
                }
                close(fd[0]);
                close(fd[1]);
                close(fd_2[0]);
                close(fd_2[1]);
                arg_count = 0;
                for (int i = 0; i < numCommands; i++)
                    free(argv[i]);
                free(commands);
                break;
            }
        }
    }
    exit(EXIT_SUCCESS);
}