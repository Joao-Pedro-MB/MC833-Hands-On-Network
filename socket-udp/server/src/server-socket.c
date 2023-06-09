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

void receive_request(int sockfd, struct Packet * packets, int * n_packets_ptr, struct sockaddr_storage * their_addr, char * s) {

    int numbytes, n_packets = *n_packets_ptr;
    socklen_t addr_len = sizeof * their_addr;

    // receives request from client
    while (n_packets < MAX_PACKET_NUMBER) {
        if ((numbytes = recvfrom(sockfd, &packets[n_packets], sizeof(struct Packet) , 0,
            (struct sockaddr *)their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }
        n_packets++;

        printf("%d\n", n_packets);

        printf("listener: got packet from %s\n",
            inet_ntop(their_addr->ss_family,
                get_in_addr((struct sockaddr *)their_addr),
                s, sizeof s));
        printf("listener: packet is %d bytes long\n", numbytes);
        packets[n_packets-1].data[packets[n_packets-1].dataSize] = '\0';
        printf("listener: packet contains \"%s\"\n", packets[n_packets-1].data);

        if (n_packets >= packets[n_packets-1].totalPackets) {
            printf("listener: all packets received\n");
            break;
        }
    }

    *n_packets_ptr = n_packets;
    printf("updated n_ptr value: %d and packes n: %d\n", *n_packets_ptr, n_packets);
}

void send_response(int sockfd, struct Packet * packets, int * n_packets_ptr, struct sockaddr_storage * their_addr, char * s) {
    printf("send_response() called\n");
    int numbytes, n_packets = *n_packets_ptr;
    printf("n_packets called: %d\n", n_packets);
    socklen_t addr_len = sizeof *their_addr;

    char * response;
    printf("talker: answering request\n");
    int is_image = answer_request(packets, n_packets, &response);
    printf("talker: response is \"%s\"\n", response);

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
                     (struct sockaddr *)their_addr, addr_len)) == -1) {
                perror("talker: sendto");
                exit(1);
            }

            printf("talker: sent %d bytes of %d containing %s\n", numbytes, packet->dataSize, packet->data);
            free(packet);
        }
    } else {

        FILE *file;
        char buffer[MAX_DGRAM_SIZE];
        size_t bytesRead, numbytes;

        // Open the image file in binary mode
        file = fopen("./server/image/silver-gull.jpg", "rb");
        if (!file) {
            printf("Error opening input image file.\n");
            exit(1);
        }

        while ((bytesRead = fread(buffer, 1, MAX_DGRAM_SIZE, file)) > 0) {
            numbytes = sendto(sockfd, buffer, bytesRead, 0,
                (struct sockaddr *)their_addr, addr_len);

            printf("talker: sent %ld bytes to %s\n", numbytes, SERVER_IP);
        }
        
        fclose(file);
    }

}

int start_server(void)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    struct sockaddr_storage their_addr;
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

    // receives request from client

    while(1){
        struct Packet packets[MAX_PACKET_NUMBER];
        int n_packets = 0;
        int * n_packets_ptr = &n_packets;

        receive_request(sockfd, packets, n_packets_ptr, &their_addr, s);
        send_response(sockfd, packets, n_packets_ptr, &their_addr, s);
    }

    close(sockfd);

    return 0;
}

