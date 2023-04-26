/*
** client.c -- a stream socket client demo
*/

#include "client-socket.h"

int initialize_socket(int * sockfd, struct addrinfo * hints, struct addrinfo * servinfo, struct addrinfo * p, int * rv, char * s) {
    printf("client inside initializing\n");
    memset(hints, 0, sizeof *hints);
    hints->ai_family = AF_UNSPEC;
    hints->ai_socktype = SOCK_STREAM;


    printf("client geting addrinfo\n");
    if ((*rv = getaddrinfo(IP, PORT, hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(*rv));
        return 1;
    }
    // loop through all the results and connect to the first we can
    printf("client looping\n");
    for(p = servinfo; p != NULL; p = p->ai_next) {

        printf("client testing socket\n");
        printf("client ai_socktype: %d\n", p->ai_socktype);
        if ((*sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(*sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(*sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo); // all done with this structure

    return 0;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int use_socket(char * request, char response[MAXDATASIZE]) {
	
    int sockfd, bytes_received;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    printf("client initializing\n");
    int err = initialize_socket(&sockfd, &hints, servinfo, p, &rv, s);
    if (err > 0) {
        return err;
    }
    printf("client sending\n");
    printf("request: %s\n\n", request);
    if (send(sockfd, request, strlen(request), 0) == -1)
        perror("send");

    printf("client receiving\n");
    if ((bytes_received = recv(sockfd, response, MAXDATASIZE - 1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    response[bytes_received] = '\0';

    printf("reposnse: %s\n\n", response);



    close(sockfd);

    return 0;
}