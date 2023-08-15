#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024

void send_file(int client_socket, const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    char *filename = strrchr(file_path, '/');
    if (filename != NULL) {
        filename++;
    } else {
        filename = (char *)file_path;
    }

    printf("%s", filename);
    send(client_socket, filename, strlen(filename), 0);

    char buffer[MAX_BUFFER_SIZE];
    size_t bytes_read;
    while (!feof(file)) {
        bytes_read = fread(buffer, 1, sizeof(buffer), file);
    }
    send(client_socket, buffer, bytes_read, 0);


    fclose(file);
}

void download_file(int client_socket) {
    char buffer[MAX_BUFFER_SIZE];
    char filename[MAX_BUFFER_SIZE];
    char text[MAX_BUFFER_SIZE];
    recv(client_socket, buffer, sizeof(buffer), 0);
    printf("Available files on the server:\n%s\n", buffer);

    printf("Enter the filename you want to download: ");
    scanf("%s", filename);
    send(client_socket, filename, sizeof(filename), 0);
    printf("1");

    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error creating file");
        return;
    }

    while (1) {
        int bytes_received = recv(client_socket, text, sizeof(text), 0);
        if (bytes_received <= 0) {
            break;
        }
        fwrite(text, 1, bytes_received, file);
    }

    fclose(file);
    printf("File '%s' downloaded successfully.\n", filename);
}

int main(int argc, char** argv) {
    int client_socket;
    struct sockaddr_in server_addr;
    char* address;
    char* port;
    if (argc != 2){
        printf("Run the program like this: a.out <IP_address:Port> <Name>");
        exit(EXIT_FAILURE);
    }

    address = strtok(argv[1], ":");
    port = strtok(NULL, ":");

    int port_num = atoi(port);

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_num);
    inet_pton(AF_INET, address, &server_addr.sin_addr);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to the server");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server.\n");

    int option;
    printf("Enter 1 to send a file, 2 to download a file: ");
    scanf("%d", &option);
    send(client_socket, &option, sizeof(option), 0);

    if (option == 1) {
        char file_path[MAX_BUFFER_SIZE];
        printf("Enter the path to the file you want to send: ");
        //fgets(file_path, MAX_BUFFER_SIZE, stdin);
        scanf("%s", file_path);
        file_path[strcspn(file_path, "\n")] = '\0';
        send_file(client_socket, file_path);
        printf("File sent successfully.\n");
    } else if (option == 2) {
        download_file(client_socket);
    } else {
        printf("Invalid option.\n");
    }


    close(client_socket);
    return 0;
}