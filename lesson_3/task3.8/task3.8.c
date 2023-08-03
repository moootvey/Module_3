#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/ipc.h>
#include <unistd.h>

#define MAX_LENGTH 255

struct message{
    long mtype;
    char mtext[255];
};

int main() {
    key_t key;
    int msgid;
    struct message msg;
    char sendText[MAX_LENGTH];

    key = ftok("chat", 'A');
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("Queue creation error!");
        exit(EXIT_FAILURE);
    }

    msg.mtype = 1;

    switch (fork()) {
        case -1:
            perror("Fork error!");
            exit(EXIT_FAILURE);
        case 0:
            while (1) {
                fgets(sendText, MAX_LENGTH, stdin);
                sendText[strcspn(sendText, "\n")] = '\0';
                if (strcmp(sendText, "exit") == 0)
                    msg.mtype = 255;
                if (msg.mtype == 255){
                    if (msgctl(msgid, IPC_RMID, NULL) == -1){
                        perror("Error deleting message queue!");
                        exit(EXIT_FAILURE);
                    }
                    exit(EXIT_SUCCESS);
                }
                strcpy(msg.mtext, sendText);
                if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
                    perror("Message sending error!");
                    exit(EXIT_FAILURE);
                }
            }
        default:
            while (1) {
                if (msgrcv(msgid, &msg, 255, 1, 0) == -1) {
                    perror("Error receiving message!");
                    exit(EXIT_FAILURE);
                }
                printf("Receive message: %s\n",msg.mtext);
            }
    }
}