#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#define MAX_PACKET_SIZE 65536

void print_packet(const unsigned char *packet, int length) {
    for (int i = 0; i < length; ++i) {
        printf("%02x ", packet[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

int main() {
    int sockfd;
    struct sockaddr_in my_addr, their_addr;
    socklen_t addr_len;
    unsigned char buffer[MAX_PACKET_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = 0; // Use any port
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        addr_len = sizeof(struct sockaddr);
        int packet_len = recvfrom(sockfd, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&their_addr, &addr_len);
        if (packet_len == -1) {
            perror("recvfrom");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        struct iphdr *ip_header = (struct iphdr *)buffer;
        struct udphdr *udp_header = (struct udphdr *)(buffer + ip_header->ihl * 4);

        printf("Received a packet from %s:%d to %s:%d\n",
               inet_ntoa(*(struct in_addr *)&ip_header->saddr),
               ntohs(udp_header->source),
               inet_ntoa(*(struct in_addr *)&ip_header->daddr),
               ntohs(udp_header->dest));

        print_packet(buffer, packet_len);
    }

    close(sockfd);
    return 0;
}