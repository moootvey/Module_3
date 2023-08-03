#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define ARG_LENGTH_MAX 50

int main(){
    //pid_t pid;
    int rv;
    while (true){
        int maxCount = 0;
        int argCount = 0;
        char progName[20];
        char args[maxCount][ARG_LENGTH_MAX];

        printf("Enter program name: ");
        scanf("%s", progName);
        printf("Enter max count of arguments: ");
        scanf("%d", &maxCount);
        printf("Enter function arguments (via 'enter')(max count of arguments = %d):\n", maxCount);

        while (argCount < maxCount) {
            scanf("%s", args[argCount]);
            argCount++;
        }

        char *argv[maxCount + 2];
        argv[0] = progName;
        for (int i = 0; i < argCount; i++){
            argv[i + 1] = args[i];
        }
        argv[argCount + 1] = NULL;

        switch (fork()) {
            case -1:
                perror("Error!");
                exit(EXIT_FAILURE);
            case 0:
                execv(progName, argv);
            default:
                wait(&rv);
                break;
        }
        //argCount = 0;
    }
    exit(EXIT_SUCCESS);
}
