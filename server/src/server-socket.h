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

#define CREATE_PROFILE 1
#define SEARCH_BATCH 2
#define LIST_ALL 3
#define FIND_PROFILE 4
#define DELETE_PROFILE 5

#define GREATER 62
#define LESS 60
#define EQUAL 122
#define GREATER_EQUAL 123
#define LESS_EQUAL 121
#define NOT_EQUAL 94


void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);
int start_server(void);
char * answer_request(char * request);

#endif // SERVER_SOCKET_H_