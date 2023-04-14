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
#define MAXDATASIZE 1000 // max number of bytes we can get at once

void *get_in_addr(struct sockaddr *sa);

int use_socket(cJSON * request);

#endif
