#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#define MAX_LENGTH 1024

int main(int argc, char** argv){
    int socketFD;
    int n;
    int len;
    char start_line[100];
    char send_text[MAX_LENGTH];
    char receive_text [MAX_LENGTH];
    char print_text [MAX_LENGTH];
    struct sockaddr_in server_address, client_address;
    int port;
    int pid;

    if (argc != 3){
        printf("Run the program like this: a.out <Port> <Name>\n");
        exit(EXIT_FAILURE);
    }

    port = atoi(argv[1]);

    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((socketFD = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("UDP socket creating error");
        exit(EXIT_FAILURE);
    }

    if (bind(socketFD, (struct sockaddr *) &server_address, sizeof(server_address)) < 0){
        perror("Preparing socket address error");
        close(socketFD);
        exit(EXIT_FAILURE);
    }

    len = sizeof(client_address);

    if ((n = recvfrom(socketFD, start_line, 101, 0, (struct sockaddr*) &client_address, &len)) < 0){
        perror("Receiving start message error");
        close(socketFD);
        exit(EXIT_FAILURE);
    }

    if (sendto(socketFD, argv[2], strlen(argv[2]), 0, (struct sockaddr*) &client_address, len) < 0){
        perror("Name sending error");
        close(socketFD);
        exit(EXIT_FAILURE);
    }

    switch (pid = fork()) {
        case -1:
            perror("Fork error");
            close(socketFD);
            exit(EXIT_FAILURE);
        case 0:{
            while(1){
                memset(receive_text, 0, MAX_LENGTH);
                if ((n = recvfrom(socketFD, receive_text, MAX_LENGTH + 1, 0, (struct sockaddr*) &client_address, &len)) < 0){
                    perror("Message receiving error");
                    close(socketFD);
                    exit(EXIT_FAILURE);
                }
                if (strcmp(receive_text, "exit\n") == 0){
                    printf("Goodbye!\n");
                    close(socketFD);
                    exit(EXIT_SUCCESS);
                }
                printf("%s: %s\n", start_line, receive_text);

            }
        }
        default:{
            while(1){
                fgets(send_text, MAX_LENGTH, stdin);
                send_text[strcspn(send_text, "\n")] = '\0';
                if (sendto(socketFD, send_text, strlen(send_text), 0, (struct sockaddr*) &client_address, len) < 0){
                    perror("Message sending error");
                    close(socketFD);
                    exit(EXIT_FAILURE);
                }
                if (strcmp(send_text, "exit\n") == 0){
                    printf("Goodbye!\n");
                    close(socketFD);
                    exit(EXIT_SUCCESS);
                }
            }
        }

    }
    close(socketFD);
    exit(EXIT_SUCCESS);
}