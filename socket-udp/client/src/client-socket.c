/*
** a UDP talker to test the code
*/

#include "client-socket.h"

int use_socket(char * request, int is_image) {   

    printf("request: %s\n", request);

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


    // sends request to server

    int totalPackets = (strlen(request) / MAX_DGRAM_SIZE) + 1;
    for (int n_packets = 0; n_packets < totalPackets; n_packets++){
        struct Packet *packet;
        packet = (struct Packet *)malloc(sizeof(struct Packet));
        packet->totalPackets = 1;
        packet->packetNumber = 0;
        packet->dataSize = strlen(request);
        memcpy(packet->data, request, packet->dataSize);

        if ((numbytes = sendto(sockfd, packet, sizeof(struct Packet), 0,
            p->ai_addr, p->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }

        free(packet);
    }



    // receives response from server
    if (is_image == 0) {
        struct Packet packets[MAX_PACKET_NUMBER];
        int n_packets = 0;
        while(n_packets < MAX_PACKET_NUMBER) {

            if ((numbytes = recvfrom(sockfd, &packets[n_packets], sizeof(struct Packet) , 0,
                p->ai_addr, &p->ai_addrlen)) == -1) {
                perror("recvfrom");
                exit(1);
            }

            n_packets++;

            if (n_packets >= packets[n_packets-1].totalPackets) {
                printf("talker: all packets received\n");
                break;
            }
        }

        receive_answer(packets, n_packets);

    } else if (is_image == 1) {

        char buffer[MAX_DGRAM_SIZE];

        FILE *file = fopen("./client/image/received.jpg", "wb");
        if (file == NULL) {
            perror("Failed to open file");
        }

        size_t bytesRead, totalBytesRead = 0;
        while ((bytesRead = recvfrom(sockfd, buffer, MAX_DGRAM_SIZE, 0, p->ai_addr, &p->ai_addrlen)) > 0) {
            size_t num_bytes_written = fwrite(buffer, sizeof(char), bytesRead, file);
            if (num_bytes_written < bytesRead) {
                perror("Failed to write data to file");
            }
            totalBytesRead += bytesRead;
            printf("Received bytesRead: %ld so total bytes are: %ld bytes\n", bytesRead,totalBytesRead);

            if (bytesRead < MAX_DGRAM_SIZE) {
                break;
            }
        }
        fclose(file);
    }

    freeaddrinfo(servinfo);

    // printf("talker: sent %d bytes of %d containing %s\n", numbytes, packet->dataSize, packet->data);

    close(sockfd);

    return 0;
}