/*
** client.c -- a stream socket client demo
*/

#include "client-socket.h"

int initialize_socket(int * sockfd, struct addrinfo * hints, struct addrinfo * servinfo, struct addrinfo * p, int * rv, char * s) {
    
    memset(hints, 0, sizeof *hints);
    hints->ai_family = AF_INET;
    hints->ai_socktype = SOCK_DGRAM;

    if ((*rv = getaddrinfo(IP, PORT, hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(*rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {

        if ((*sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to create socket\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);

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
    struct addrinfo hints = {0}, *servinfo = NULL, *p = NULL;

	/*struct designed to handle IPv4 and IPv6 structures*/
	struct sockaddr_storage their_addr; // connector's address information

	socklen_t addr_len; // connectors expected address size
	char s[INET6_ADDRSTRLEN];// array with a IPv6 address size
	int rv=0;
	int numbytes;

    printf("client initializing\n");
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;


    /*rv is just to cach possible errors of the function*/
	rv = getaddrinfo(NULL /*host name like www.example.com or IP*/, 
					 PORT /*service type like HTTP or PORT number*/, 
					 &hints /*filter to possible answers*/, 
					 &servinfo /*the actual answer of the function with a pointer to a linked-list of result*/);
	if (rv != 0) {
		/*gai_strerror returns a string explaining the error value returned by
		the getaddrinfo funtion*/
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {

        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to create socket\n");
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    printf("request: %s\n", request);
    if (numbytes = sendto(sockfd, request, strlen(request), 0, p->ai_addr, p->ai_addrlen) == -1) {
        perror("sendto");
        exit(1);
    }

    printf("client receiving\n");
    addr_len = sizeof their_addr;
    bytes_received = recvfrom(sockfd, response, MAXDATASIZE - 1, 0, &their_addr, &addr_len);
    if (bytes_received == -1) {
        perror("recv");
        exit(1);
    }

    response[bytes_received] = '\0';

    printf("response: %s\n\n", response);

    close(sockfd);

    return 0;
}
