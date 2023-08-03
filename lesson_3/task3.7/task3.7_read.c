#include <stdio.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

struct message{
    long mtype;
    char mtext[255];
};

int main() {
    //sleep(10);
    key_t key;
    int msgid;
    struct message msg;
    int count = 0;
    int flag;

    key = ftok("key.txt", 'A');
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1){
        perror("Queue creation error.");
        exit(EXIT_FAILURE);
    }

    if (msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0) == -1){
        perror("Error receiving message.");
        exit(EXIT_FAILURE);
    }

    count = atoi(msg.mtext);

    for(int i = 0; i != count; i++){
        if (msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0) == -1){
            perror("Error receiving message.");
            exit(EXIT_FAILURE);
        }
        printf("%d) Receive message type %ld: %s\n", i + 1, msg.mtype, msg.mtext);
        if (atoi(msg.mtext) == 255){
            flag = atoi(msg.mtext);
            msg.mtype = flag;
            break;
        }
    }

    exit(EXIT_SUCCESS);
}
