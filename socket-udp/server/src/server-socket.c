/*
** server.c -- a stream socket server demo
*/

#include "server-socket.h"

int initialize_socket(struct addrinfo * hints, struct addrinfo * servinfo, struct addrinfo * p, int * rv) {

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET6; // set to AF_INET to use IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;

	}

	if (p == NULL) {
		fprintf(stderr, "server: failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo);

    return 0;
}

void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void clean_zombies(struct sigaction * sa) {
	/* reap all dead/zombie processes to avoid PID and dad memory spaces */
	sa->sa_handler = sigchld_handler;
	sigemptyset(&(sa->sa_mask));
	sa->sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}
}

void execute_request(int * socket_execution_thread, socklen_t * sin_size, struct sockaddr_storage * their_addr) {
	char request[MAXDATASIZE];

	if ((bytes_received = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
		(struct sockaddr *)&their_addr, *sin_size)) == -1) {
		perror("recvfrom");
		exit(1);
	}

	request[bytes_received] = '\0';

	printf("listener: got packet from %s\n",
		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s));
	printf("listener: packet is %d bytes long\n", bytes_received);
	printf("listener: packet contains \"%s\"\n", request);

	close(sockfd);

	// TODO: check client response example
	char * response = answer_request(request);

	printf("server: response '%s'\n", response);

    if (send(*socket_execution_thread, response, strlen(response), 0) == -1)
        perror("send");
	close(*socket_execution_thread);
	exit(0);
}

int start_server(void) {
	int socket_listener_thread;

	/*
	struct addrinfo {
		int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
		int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
		int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
		int              ai_protocol;  // use 0 for "any"
		size_t           ai_addrlen;   // size of ai_addr in bytes
		struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
		char            *ai_canonname; // full canonical hostname
		struct addrinfo *ai_next;      // linked list, next node
	};
	*/
	struct addrinfo hints = {0}, *servinfo = NULL, *p = NULL;

	/*struct designed to handle IPv4 and IPv6 structures*/
	struct sockaddr_storage their_addr; // connector's address information

	socklen_t sin_size; // connectors expected adress size
	struct sigaction sa; // signal action struct to handle a specific signal
	char s[INET6_ADDRSTRLEN];// array with a IPv6 address size
	int rv=0;
	printf("server initializing...\n");
	int err = initialize_socket(&hints, servinfo, p, &rv);
    if (err > 0) {
        return err;
    }

	printf("server cleaning zombies...\n");
	clean_zombies(&sa);

	printf("server: waiting to recvfrom...\n");

	sin_size = sizeof their_addr;

	execute_request(&socket_execution_thread, &sin_size, &their_addr);
	
	return 0;
}
