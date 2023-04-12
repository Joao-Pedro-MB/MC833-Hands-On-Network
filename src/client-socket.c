/*
** client.c -- a stream socket client demo
*/

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

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 1000 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int start_and_connect(int * sockfd, addrinfo * hints, servinfo, p, int * rv, char * s){
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure
}

int main(int argc, char *argv[]) {
	int sockfd, bytes_received;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 2) {
		fprintf(stderr,"usage: client hostname\n");
		exit(1);
	}

	int err = start_and_connect(&sockfd, &hints, servinfo, p, &rv, s);

	// FILE *fp = fopen("./text/example.json", "r");
	
	// if (!fp) {
	// 	printf("Failed to open file\n");
	// 	return 1;
	// }

	// // Read the JSON data from the file
	// fseek(fp, 0, SEEK_END);
	// long file_size = ftell(fp);
	// fseek(fp, 0, SEEK_SET);
	// char *json_data = malloc(file_size + 1);
	// fread(json_data, 1, file_size, fp);
	// fclose(fp);

	// printf("%s\n", json_data);


	if (send(sockfd, json_data, strlen(json_data), 0) == -1)
		perror("send");

	free(json_data);


	bytes_received = recv(sockfd, buf, MAXDATASIZE-1, 0);
			
	if (bytes_received == -1) {
		perror("recv");
		exit(1);
	}

	buf[bytes_received] = '\0';

	printf("client: received '%s'\n",buf);

	close(sockfd);

	return 0;
}
