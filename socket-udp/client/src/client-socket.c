/*
** a UDP talker to test the code
*/

#include "client-socket.h"

void extractValue(const char* str, char* value) {
    const char* valueStart = strstr(str, "\"value\":\"");
    if (valueStart == NULL) {
        strcpy(value, "");  // Value field not found
        return;
    }

    valueStart += 9;  // Move pointer to start of the value

    const char* valueEnd = strchr(valueStart, '\"');
    if (valueEnd == NULL) {
        strcpy(value, "");  // Invalid string format
        return;
    }

    strncpy(value, valueStart, valueEnd - valueStart);
    value[valueEnd - valueStart] = '\0';  // Null-terminate the extracted value
}

void send_request(int sockfd, struct addrinfo * p, char * request) {
    printf("Sending request: %s\n", request);
    long int numbytes = 0, totalPackets = (strlen(request) / MAX_DGRAM_SIZE) + 1;

    printf("totalPackets: %ld\n", totalPackets);
    for (int n_packets = 0; n_packets < totalPackets; n_packets++){
        printf("\n\ninside loop: %d\n\n\n", n_packets);
        struct Packet *packet;
        packet = (struct Packet *)malloc(sizeof(struct Packet));
        packet->totalPackets = totalPackets;
        packet->packetNumber = n_packets;
        packet->dataSize = MAX_DGRAM_SIZE;
        if (n_packets == totalPackets - 1) {
            packet->dataSize = strlen(request) - (n_packets * MAX_DGRAM_SIZE);
        }
        memcpy(packet->data, request + (n_packets * MAX_DGRAM_SIZE), packet->dataSize);
        printf("packet: %s\n", packet->data);
        printf("packet->dataSize: %d\n", packet->dataSize);
        if ((numbytes = sendto(sockfd, packet, sizeof(struct Packet), 0,
            p->ai_addr, p->ai_addrlen)) == -1) {
            perror("talker: sendto");
            exit(1);
        }

        free(packet);
    }
}

void receive_image(int sockfd, struct addrinfo * p, char * request) {
    char buffer[MAX_DGRAM_SIZE], image_path[400], image_name[200];
    size_t bytesRead = 0, totalBytesRead = 0;

    extractValue(request, image_name);
    sprintf(image_path, "./client/image/%s.jpg", image_name);

    printf("request: %s\n", request);

    FILE *file = fopen(image_path, "wb");
    if (file == NULL) {
        perror("Failed to open file");
    }

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

void receive_response(int sockfd, struct addrinfo * p) {
    struct Packet packets[MAX_PACKET_NUMBER];
    int numbytes = 0, n_packets = 0;
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

    printf("n_packets: %d\n", n_packets);
    printf("vai entrar no receive nswer\n");
    receive_answer(packets, n_packets);
}

int use_socket(char * request, int is_image, int argc, char *argv[]) {   

    printf("responding\n");

    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char *ip = SERVER_IP;

    if (argc == 2) {
        ip = argv[1];
        printf("ip: %s\n", ip);
    }


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(ip, PORT, &hints, &servinfo)) != 0) {
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
    send_request(sockfd, p, request);

    // receives response from server
    if (is_image == 0) {
        receive_response(sockfd, p);

    } else if (is_image == 1) {
        receive_image(sockfd, p, request);
    }

    freeaddrinfo(servinfo);

    close(sockfd);

    return 0;
}