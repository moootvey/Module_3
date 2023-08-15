#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>

#define QUEUE_NAME "/my_queue"
#define MAX_MESSAGE_SIZE 255

int main() {
    mqd_t msgid;
    char text[MAX_MESSAGE_SIZE];
    int count = 0;
    int flag;

    // Open the message queue
    msgid = mq_open(QUEUE_NAME, O_RDWR);
    if (msgid == -1){
        perror("Queue open error.");
        exit(EXIT_FAILURE);
    }
    printf("1");
    // Receive the count of numbers from the message queue
    if (mq_receive(msgid, text, MAX_MESSAGE_SIZE, NULL) == -1){
        perror("Error receiving message.");
        exit(EXIT_FAILURE);
    }

    count = atoi(text);
    printf("2");
    for(int i = 0; i != count; i++){
        if (mq_receive(msgid, text, MAX_MESSAGE_SIZE, NULL) == -1){
            perror("Error receiving message.");
            exit(EXIT_FAILURE);
        }
        text[MAX_MESSAGE_SIZE - 1] = '\0'; // Null-terminate the received message
        printf("%d) Receive message: %s\n", i + 1, text);
        if (atoi(text) == 255){
            break;
        }
    }

    // Close the message queue
    if (mq_close(msgid) == -1){
        perror("Error closing message queue!");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}