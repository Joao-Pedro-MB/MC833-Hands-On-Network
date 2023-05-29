#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cJSON.h>
//#include <b64/cdecode.h>

#define SERVER_IP "::1" // the fixed IP that the server is running on
#define CLIENT_IP "::1" // the fixed IP that the client is running
#define SERVER_PORT "3490"
#define CLIENT_PORT "3491" // the port client will be connecting
#define MAX_DGRAM_SIZE 4096 // max number of bytes we can get at once
#define MAX_PACKET_NUMBER 100

#define CREATE_PROFILE 1
#define SEARCH_BATCH 2
#define LIST_ALL 3
#define FIND_PROFILE 4
#define ADD_PICTURE 5
#define DELETE_PROFILE 6

struct Packet {
    int packetNumber;
    int totalPackets;
    int dataSize;
    char data[MAX_DGRAM_SIZE];
};

void *get_in_addr(struct sockaddr *sa);

int use_socket(char * request, int is_image);

size_t get_image(char * path, char * image);

void receive_answer(struct Packet packets[], int num_packets);

int start_listener();

#endif
