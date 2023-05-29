/*
** a UDP talker to test the code
*/

#include "client-socket.h"

#define MAX_DGRAM_SIZE 4096

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
    printf("talker: request: %s\n", request);

    // wrap packet
    int sizeofMessage = strlen(request);
    int totalPackets = sizeofMessage / MAX_DGRAM_SIZE + 1;

    for (int packetNumber = 0 ; packetNumber < totalPackets; packetNumber++) {
        struct Packet packet;
        packet.packetNumber = packetNumber;
        packet.totalPackets = totalPackets;
        packet.dataSize = MAX_DGRAM_SIZE;
        if (packetNumber == totalPackets - 1) {
            packet.dataSize = sizeofMessage % MAX_DGRAM_SIZE;
        }
        memcpy(packet.data, request + packetNumber * MAX_DGRAM_SIZE, packet.dataSize);
        
        printf("\n\n\n\ntalker: packetNumber: %d\n", packetNumber);
        printf("talker: totalPackets: %d\n", totalPackets);
        printf("talker: dataSize: %d\n", packet.dataSize);
        printf("talker: message size: %d\n", sizeofMessage);
        printf("talker: MAX_DGRAM_SIZE: %d\n", MAX_DGRAM_SIZE);
        printf("talker: data: %s\n\n\n\n\n", packet.data);


        if ((numbytes = sendto(sockfd, &packet, sizeof(struct Packet), 0,
                 p->ai_addr, p->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }
    }

    freeaddrinfo(servinfo);

    printf("talker: sent %d bytes to %s\n", numbytes, SERVER_IP);
    close(sockfd);

    return 0;
}