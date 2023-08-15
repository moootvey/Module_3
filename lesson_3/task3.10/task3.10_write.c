#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>
#include <time.h>

#define QUEUE_NAME "/my_queue"
#define MAX_MESSAGE_SIZE 255

int main() {
    char text[255];
    srand(time(NULL));
    mqd_t msgid;
    int count = 0;
    int numbers[100];
    int num;
    FILE* file;

    // Create the message queue
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(text);
    attr.mq_curmsgs = 0;
    mq_unlink(QUEUE_NAME);
    msgid = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
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

    // Send the count of numbers in the message queue
    sprintf(text, "%d", count);
    if (mq_send(msgid, text, strlen(text), 0) == -1){
        perror("Message sending error");
        exit(EXIT_FAILURE);
    }

    // Send the numbers one by one in the message queue
    for (int i = 0; i != count; i++){
        sleep(1);
        sprintf(text, "%d", numbers[i]);
        printf("%s\n", text);
        if (mq_send(msgid, text, strlen(text), 0) == -1){
            perror("Message sending error");
            exit(EXIT_FAILURE);
        }
        if (numbers[i] == 255) {
            // Remove the message queue if 255 is encountered
            if (mq_close(msgid) == -1){
                perror("Error closing message queue!");
                exit(EXIT_FAILURE);
            }
            if (mq_unlink(QUEUE_NAME) == -1){
                perror("Error deleting message queue!");
                exit(EXIT_FAILURE);
            }
            break;
        }
    }

    printf("Message send!\n");

    exit(EXIT_SUCCESS);
}