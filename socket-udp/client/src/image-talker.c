/*
** a UDP talker to test the code
*/

#include "client-socket.h"

#define MAX_DGRAM_SIZE 4096


struct Packet {
    int packetNumber;
    int totalPackets;
    int dataSize;
    int command;
    char data[MAX_DGRAM_SIZE];
};

int use_socket(char * request, int is_image)
{   
    printf("Dentro do use socket\n");
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }

    printf("talker: sending request\n");

    FILE *file = fopen("./client/image/silver-gull.jpg", "rb");
    if (file == NULL) {
        perror("Failed to open file");
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char buffer[MAX_DGRAM_SIZE];
    size_t num_bytes_read, total_bytes_sent = 0;

    while((num_bytes_read = fread(buffer, sizeof(char), MAX_DGRAM_SIZE, file)) > 0) {
        numbytes = sendto(sockfd, buffer, num_bytes_read, 0,
                p->ai_addr, p->ai_addrlen);

        if (numbytes == -1) {
            perror("talker: sendto");
            exit(1);
        }
    }

    freeaddrinfo(servinfo);

    printf("talker: sent %d bytes to %s\n", numbytes, SERVER_IP);
    fclose(file);
    close(sockfd);

    return 0;
}