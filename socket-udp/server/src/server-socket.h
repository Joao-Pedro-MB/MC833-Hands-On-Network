/*
** server-socket.h -- a header file for stream socket server demo
*/

#ifndef SERVER_SOCKET_H_
#define SERVER_SOCKET_H_

#include <limits.h>
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

#define SERVER_PORT "3490"  // the port users will be connecting to
#define CLIENT_PORT "3491" // the port for the client to connect to
#define CLIENT_IP "::1"
#define SERVER_IP "::1" // the hardcoded IP for debugging purposes, will be deleted soon

#define CREATE_PROFILE 1
#define SEARCH_BATCH 2
#define LIST_ALL 3
#define FIND_PROFILE 4
#define GET_IMAGE 5
#define DELETE_PROFILE 6

#define GREATER 62
#define LESS 60
#define EQUAL 122
#define GREATER_EQUAL 123
#define LESS_EQUAL 121
#define NOT_EQUAL 94

#define MAX_PACKET_NUMBER 100
#define MAX_DGRAM_SIZE 4096

struct Packet{
    int packetNumber;
    int totalPackets;
    int dataSize;
    char data[MAX_DGRAM_SIZE];
};


// helper functions
cJSON * access_database();

int convert_ASCII(char * input);

int compare_strings(cJSON* base, cJSON* operation, cJSON* target);

int compare_ints(cJSON* base, cJSON* operation, cJSON* target);

int find_word(char * data, cJSON * operation, char * target);

int write_database(cJSON* database);

char * create_error_response(int status, const char * message);

char * format_response(int status, char* message);

char * compare_database(cJSON* field, cJSON* operation, cJSON* value, cJSON* profiles_array);

// server functions
void sigchld_handler(int s);

void * get_in_addr(struct sockaddr *sa);

int start_server(void);

int answer_request(struct Packet packets[100], int num_packets, char ** json_response);

#endif // SERVER_SOCKET_H_