#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>

#define MAX_BUFFER_SIZE 1024

void save_file(int client_socket, const char *cloud_path) {
    char path[128];
    strcpy(path, cloud_path);
    char buffer[MAX_BUFFER_SIZE];
    char text[MAX_BUFFER_SIZE];

    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        perror("Error receiving filename");
        return;
    }
    buffer[bytes_received] = '\0';

    strcat(path, buffer);

    FILE *file = fopen(path, "w");
    if (!file) {
        perror("Error creating file");
        return;
    }

    while (1) {
        bytes_received = recv(client_socket, text, sizeof(text), 0);
        if (bytes_received <= 0) {
            break;
        }
        fwrite(text, 1, bytes_received, file);
    }

    fclose(file);
    printf("File '%s' saved successfully.\n", buffer);
}

void send_file_list(int client_socket, const char *cloud_path) {
    struct dirent *dir_entry;
    DIR *dir = opendir(cloud_path);
    if (!dir) {
        perror("Error opening directory");
        return;
    }

    char file_list[MAX_BUFFER_SIZE] = "";
    while ((dir_entry = readdir(dir)) != NULL) {
        if (dir_entry->d_type == DT_REG) {
            strcat(file_list, dir_entry->d_name);
            strcat(file_list, "\n");
        }
    }
    closedir(dir);

    send(client_socket, file_list, strlen(file_list), 0);
}

void send_file(int client_socket, const char *filename, const char *cloud_path) {
    char path[128];
    strcpy(path, cloud_path);
    char buf[MAX_BUFFER_SIZE];
    sleep(1);
    strcat(path, filename);

    FILE *file = fopen(path, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    char buffer[MAX_BUFFER_SIZE];
    size_t bytes_read;
    while (!feof(file)) {
        bytes_read = fread(buffer, 1, sizeof(buffer), file);
    }
    send(client_socket, buffer, bytes_read, 0);

    fclose(file);
}

int main(int argc, char *argv[]) {
    int server_socket, client_socket;
    int port;
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port> <cloud_path>\n", argv[0]);
        return 1;
    }
    port = atoi(argv[1]);

    char *cloud_path = (char *)malloc(strlen(argv[2]) + 1);
    if (cloud_path == NULL) {
        perror("Error allocating memory");
        return 1;
    }
    strcpy(cloud_path, argv[2]);
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", port);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        int option;
        recv(client_socket, &option, sizeof(option), 0);

        if (option == 1) {
            save_file(client_socket, cloud_path);
        } else if (option == 2) {
            send_file_list(client_socket, cloud_path);
            char filename[MAX_BUFFER_SIZE];
            recv(client_socket, filename, sizeof(filename), 0);
            send_file(client_socket, filename, cloud_path);
            printf("File '%s' sent successfully.\n", filename);
        } else {
            printf("Invalid option.\n");
        }

        close(client_socket);
    }
    free(cloud_path);
    close(server_socket);
    return 0;
}