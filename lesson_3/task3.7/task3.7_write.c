#include <stdio.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

struct message{
    long mtype;
    char mtext[255];
};

int main() {
    srand(time(NULL));
    key_t key;
    int msgid;
    struct message msg;
    int val;
    int count = 0;
    int numbers[100];
    int num;
    FILE* file;

    key = ftok("key.txt", 'A');
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1){
        perror("Queue creation error.");
        exit(EXIT_FAILURE);
    }

    file = fopen("val.txt", "r");
    if (file == NULL){
        perror("File open error!");
        exit(EXIT_FAILURE);
    }
    while (fscanf(file, "%d", &num) == 1){
        numbers[count] = num;
        count++;
    }

    fclose(file);

    msg.mtype = 1;
    sprintf(msg.mtext, "%d", count);
    if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1){
        perror("Message sending error");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i != count; i++){
        sleep(1);
        sprintf(msg.mtext, "%d", numbers[i]);
        if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1){
            perror("Message sending error");
            exit(EXIT_FAILURE);
        }
        if (numbers[i] == 255) {
            msg.mtype = numbers[i];
            if (msg.mtype == 255) {
                if (msgctl(msgid, IPC_RMID, NULL) == -1){
                    perror("Error deleting message queue!");
                    exit(EXIT_FAILURE);
                }
                break;
            }
        }
    }

    printf("Message send!\n");

    exit(EXIT_SUCCESS);
}
