/*
** server.c -- a stream socket server demo
*/

#include "client-socket.h"

#define MAX_DGRAM_SIZE 4096
#define MAX_PACKET_NUMBER 100

struct Packet {
    int packetNumber;
    int totalPackets;
    int dataSize;
	int command;
    char data[MAX_DGRAM_SIZE];
};

#define MYPORT "4950"    // the port users will be connecting to

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int start_server(void)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(CLIENT_IP, CLIENT_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    printf("listener: waiting to recvfrom...\n");

    addr_len = sizeof(their_addr);
char buffer[MAX_DGRAM_SIZE];
    FILE *file = fopen("./server/image/received.jpg", "wb");
    if (file == NULL) {
        perror("Failed to open file");
    }

    size_t num_bytes_received, total_bytes_received = 0;
    while ((num_bytes_received = recvfrom(sockfd, buffer, MAX_DGRAM_SIZE, 0, (struct sockaddr *)&their_addr, &addr_len)) > 0) {
        size_t num_bytes_written = fwrite(buffer, sizeof(char), num_bytes_received, file);
        if (num_bytes_written < num_bytes_received) {
            perror("Failed to write data to file");
        }
        total_bytes_received += num_bytes_received;
        printf("Received %ld bytes\n", total_bytes_received);
    }
    fclose(file);
    close(sockfd);

    return 0;
}