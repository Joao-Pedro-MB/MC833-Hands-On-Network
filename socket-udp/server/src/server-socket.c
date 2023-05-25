/*
** server.c -- a stream socket server demo
*/

#include "server-socket.h"

int initialize_socket(int * socket_listener_thread, struct addrinfo * hints, struct addrinfo * servinfo, struct addrinfo * p, struct sockaddr_storage * their_addr, unsigned int * addr_len, int * rv) {

	/* hints is a struct that define the parameters of addrinfo we are 
	willing to accept like the following */
	memset(hints, 0, sizeof *hints);
	hints->ai_family = AF_INET6; /*we dont care if IPv4 or IPv6*/
	hints->ai_socktype = SOCK_DGRAM; /*we want a TCP connection*/
	hints->ai_flags = AI_PASSIVE; /* use my IP where I do not define one like a
									getaddrinfo with a NULL first field, as below */

	/*rv is just to cach possible errors of the function*/
	*rv = getaddrinfo(NULL /*host name like www.example.com or IP*/, 
					 PORT /*service type like HTTP or PORT number*/, 
					 hints /*filter to possible answers*/, 
					 &servinfo /*the actual answer of the function with a pointer to a linked-list of result*/);
	if (*rv != 0) {
		/*gai_strerror returns a string explaining the error value returned by
		the getaddrinfo funtion*/
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(*rv));
		return 1;
	}

	/* loop through all the results (possible conections) and bind to the first we can
		in this exemple case we will bind to a PORT in our own machine */
	for(p = servinfo; p != NULL; p = p->ai_next) {

		/*returns a file descriptor for the socket*/
		*socket_listener_thread = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (*socket_listener_thread == -1) {
			perror("server: socket");
			continue;
		}

		/* once the socket is found and configured we try to bind or socket to a 
		socket address i.e. IPv4 or IPv6 address + PORT
		OBS: if we use connect we will not need to bind as we dont care which port or socket is runing on */
		if (bind(*socket_listener_thread, p->ai_addr, p->ai_addrlen) == -1) {
			close(*socket_listener_thread);
			perror("server: bind");
			continue;
		}

		break;
	}

	/* once or socket is binded we can free all the possible adresses we could use*/
	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

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

char * execute_request(int * socket_listener_thread, int * addr_len, int * their_addr) {
	
	char request[MAXDATASIZE];

	int bytes_received = recvfrom(*socket_listener_thread, request, MAXDATASIZE - 1 , 0, their_addr, addr_len);
		
	if (bytes_received == -1) {
		perror("recvfrom");
		exit(1);
	}

	request[bytes_received] = '\0';

	char * response = answer_request(request);

	return response;

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

	socklen_t addr_len; // connectors expected address size
	struct sigaction sa; // signal action struct to handle a specific signal
	char s[INET6_ADDRSTRLEN];// array with a IPv6 address size
	int rv=0;
	int numbytes;

	printf("server initializing...\n");
	// int err = initialize_socket(&socket_listener_thread, &hints, servinfo, p, &their_addr, &addr_len, &rv);
    
	// if (err > 0) {
    //     return err;
    // }

	/* hints is a struct that define the parameters of addrinfo we are 
	willing to accept like the following */
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET6; /*we dont care if IPv4 or IPv6*/
	hints.ai_socktype = SOCK_DGRAM; /*we want a TCP connection*/
	hints.ai_flags = AI_PASSIVE; /* use my IP where I do not define one like a
									getaddrinfo with a NULL first field, as below */

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

	/* loop through all the results (possible conections) and bind to the first we can
		in this exemple case we will bind to a PORT in our own machine */
	for(p = servinfo; p != NULL; p = p->ai_next) {

		/*returns a file descriptor for the socket*/
		socket_listener_thread = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (socket_listener_thread == -1) {
			perror("server: socket");
			continue;
		}

		/* once the socket is found and configured we try to bind or socket to a 
		socket address i.e. IPv4 or IPv6 address + PORT
		OBS: if we use connect we will not need to bind as we dont care which port or socket is runing on */
		if (bind(socket_listener_thread, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_listener_thread);
			perror("server: bind");
			continue;
		}

		break;
	}

	/* once or socket is binded we can free all the possible adresses we could use*/
	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	printf("server cleaning zombies...\n");
	clean_zombies(&sa);

	printf("server: waiting to recvfrom...\n");

	/* define size of address and accept the connection casting the "their_addr" sockaddr_storage structure to the right struct*/
	addr_len = sizeof their_addr;
	char * response = execute_request(&socket_listener_thread, &addr_len, (struct sockaddr *)&their_addr);

	inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
	printf("server: got packet from %s\n", s);

   	if (numbytes = sendto(socket_listener_thread, response, strlen(response), 0, p->ai_addr, p->ai_addrlen) == -1) {
        perror("sendto");
        exit(1);
    }

	close(socket_listener_thread);

	return 0;
}
