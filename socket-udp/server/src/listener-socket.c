/*
** server.c -- a stream socket server demo
*/

#include "server-socket.h"


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

    if ((rv = getaddrinfo(NULL, SERVER_PORT, &hints, &servinfo)) != 0) {
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

    addr_len = sizeof their_addr;




    // receives request from client

    struct Packet packets[MAX_PACKET_NUMBER];
    int n_packets = 0;

    while (n_packets < MAX_PACKET_NUMBER) {
        if ((numbytes = recvfrom(sockfd, &packets[n_packets], sizeof(struct Packet) , 0,
            (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }
        n_packets++;

        printf("listener: got packet from %s\n",
            inet_ntop(their_addr.ss_family,
                get_in_addr((struct sockaddr *)&their_addr),
                s, sizeof s));
        printf("listener: packet is %d bytes long\n", numbytes);
        packets[n_packets-1].data[packets[n_packets-1].dataSize] = '\0';
        printf("listener: packet contains \"%s\"\n", packets[n_packets-1].data);

        if (n_packets >= packets[n_packets-1].totalPackets) {
            printf("listener: all packets received\n");
            break;
        }
    }




    // sends response to client

    char * response;
    int is_image = answer_request(packets, n_packets, response);
    printf("listener: response is \"%s\"\n", response);

    int totalPackets = (strlen(response) / MAX_DGRAM_SIZE) + 1;

    if (is_image == 0) {
        for (int n_packets = 0; n_packets < totalPackets; n_packets++){
            struct Packet *packet;
            packet = (struct Packet *)malloc(sizeof(struct Packet));
            packet->totalPackets = totalPackets;
            packet->packetNumber = n_packets;
            packet->dataSize = MAX_DGRAM_SIZE;
            if (n_packets == totalPackets - 1) {
                packet->dataSize = strlen(response) - (n_packets * MAX_DGRAM_SIZE);
            }
            memcpy(packet->data, response + (n_packets * MAX_DGRAM_SIZE), packet->dataSize);

            if ((numbytes = sendto(sockfd, packet, sizeof(struct Packet), 0,
                     (struct sockaddr *)&their_addr, addr_len)) == -1) {
                perror("talker: sendto");
                exit(1);
            }

            printf("talker: sent %d bytes of %d containing %s\n", numbytes, packet->dataSize, packet->data);
            free(packet);
        }
    } else {
        // passsar a logica do image talker que ta num arquivo do cliente para esse lado aqui
    }

    close(sockfd);

    return 0;
}

