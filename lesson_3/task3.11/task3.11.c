#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LENGTH 255
#define QUEUE_SEND "/queue_one"
#define QUEUE_RECEIVE "/queue_two"

int main() {
    struct mq_attr attr;
    char sendText[MAX_LENGTH];
    char recvText[MAX_LENGTH];
    int must_stop = 0;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_LENGTH;
    attr.mq_curmsgs = 0;

    switch (fork()) {
        case -1:
            perror("Fork error!");
            exit(EXIT_FAILURE);

        case 0:{
            mqd_t ds_send;
            ds_send = mq_open(QUEUE_SEND, O_CREAT | O_WRONLY, 0666, &attr);
            if (ds_send == (mqd_t)-1) {
                perror("Queue creation error!");
                exit(EXIT_FAILURE);
            }
            while (!must_stop) {
                fgets(sendText, MAX_LENGTH, stdin);
                sendText[strcspn(sendText, "\n")] = '\0';
                if (strcmp(sendText, "exit") == 0) {
                    must_stop = 1;
                }
                if (mq_send(ds_send, sendText, MAX_LENGTH, 1) == -1) {
                    perror("Message sending error!");
                    exit(EXIT_FAILURE);
                }
            }
            mq_close(ds_send);
            mq_unlink(QUEUE_SEND);
            break;
        }

        default:{
            //wait(NULL);
            mqd_t ds_receive;
            ds_receive = mq_open(QUEUE_RECEIVE, O_RDONLY);
            if (ds_receive == (mqd_t)-1) {
                perror("Queue creation error!");
                exit(EXIT_FAILURE);
            }
            unsigned int prio;
            while (!must_stop) {
                if (mq_receive(ds_receive, recvText, MAX_LENGTH, &prio) == -1) {
                    perror("Error receiving message!");
                    exit(EXIT_FAILURE);
                }
                printf("Received message: %s\n", recvText);
                if (strcmp(recvText, "exit") == 0) {
                    must_stop = 1;
                }
            }
            mq_close(ds_receive);
            mq_unlink(QUEUE_RECEIVE);
            break;
        }
    }

    return 0;
}