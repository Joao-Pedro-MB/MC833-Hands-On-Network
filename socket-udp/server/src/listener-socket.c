/*
** server.c -- a stream socket server demo
*/

#include "server-socket.h"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int start_server(void)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    printf("listener: waiting to recvfrom...\n");

    addr_len = sizeof their_addr;
	struct Packet packets[MAX_PACKET_NUMBER];

	// receive packet
	int packetNumber = 0;
	while (packetNumber < MAX_PACKET_NUMBER) {

		printf("listener: packet number %d\n", packetNumber);

    	if ((numbytes = recvfrom(sockfd, &packets[packetNumber], sizeof(struct Packet) , 0,
        	(struct sockaddr *)&their_addr, &addr_len)) == -1) {
        	perror("recvfrom");
        	exit(1);
    	}

		packetNumber++;

		printf("listener: packet number %d and totalpackets: %d\n", packetNumber, packets[packetNumber-1].totalPackets);

    	printf("listener: got packet from %s:%d\n",
        	inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s),
            ntohs(((struct sockaddr_in *)&their_addr)->sin_port));

    	printf("listener: packet is %d bytes long\n", numbytes);
    	printf("listener: packet contains \"%ld\"\n", strlen(packets[packetNumber-1].data));

		if (packetNumber >= packets[packetNumber-1].totalPackets) {
			break;
		}
	}

    char * response = answer_request(packets, packetNumber);

    if ((numbytes = sendto(sockfd, response, strlen(response), 0,
             (struct sockaddr *)&their_addr, addr_len)) == -1) {
        perror("talker: sendto");
        exit(1);
    }

    close(sockfd);

    return 0;
}