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

#define IP "127.0.0.1" // the fixed IP that the server is running on
#define PORT "3490" // the port client will be connecting to
#define MAXDATASIZE 10000 // max number of bytes we can get at once

#define CREATE_PROFILE 1
#define SEARCH_BATCH 2
#define LIST_ALL 3
#define FIND_PROFILE 4
#define DELETE_PROFILE 5

void *get_in_addr(struct sockaddr *sa);

int use_socket(char * request, char response[MAXDATASIZE]);

#endif
