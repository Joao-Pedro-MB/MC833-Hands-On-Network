/*
** server-socket.h -- a header file for stream socket server demo
*/

#ifndef SERVER_SOCKET_H_
#define SERVER_SOCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <cJSON.h>

#define PORT "3490"  // the port users will be connecting to
# define IP "127.0.0.1" // the hardcoded IP for debugging purposes, will be deleted soon
#define BACKLOG 10   // how many pending connections queue will hold
#define MAXDATASIZE 1024 // max number of bytes we can get at once

void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);
int use_server(void);
void test_server(void);

#endif // SERVER_SOCKET_H_