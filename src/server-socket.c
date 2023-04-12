/*
** server.c -- a stream socket server demo
*/

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

#define PORT "3490"  // the port users will be connecting to
# define IP "127.0.0.1" // the hardcoded IP for debugging purposes, will be deleted soon
#define BACKLOG 10	 // how many pending connections queue will hold
#define MAXDATASIZE 1000 // max number of bytes we can get at once

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

int main(void) {
	int socket_listener_thread, socket_execution_thread;  // listen, new connection

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
	struct addrinfo hints, *servinfo, *p;

	/*struct designed to handle IPv4 and IPv6 structures*/
	struct sockaddr_storage their_addr; // connector's address information

	socklen_t sin_size; // connectors expected adress size
	struct sigaction sa; // signal action struct to handle a specific signal
	int yes=1;
	char s[INET6_ADDRSTRLEN];// array with a IPv6 address size
	int rv;

	/* hitnts is a struct that define the parameters of addrinfo we are 
	willing to accept like the following */
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; /*we dont care if IPv4 or IPv6*/
	hints.ai_socktype = SOCK_STREAM; /*we want a TCP connection*/
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
		
		/* defines socket usage as in this case we are setting a reusage of address policy
		to the socket level and set it as active by passing the "yes" value */
		if (setsockopt(socket_listener_thread, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
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

	/* start to listen to that port for incoming requests */
	if (listen(socket_listener_thread, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	/* reap all dead/zombie processes to avoid PID and dad memory spaces */
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	/* handle incoming requests and create threads to serve them if accepted */
	while(1) {  // main accept() loop

		/* define size of address and accept the connection casting the "their_addr" sockaddr_storage structure to the right struct*/
		sin_size = sizeof their_addr;
		socket_execution_thread = accept(socket_listener_thread, (struct sockaddr *)&their_addr, &sin_size);
		if (socket_execution_thread == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
		printf("server: got connection from %s\n", s);

		if (!fork()) { // this is the child process
			close(socket_listener_thread); // child doesn't need the listener

			printf("Entrou\n");

			char buf[MAXDATASIZE];

			/* int recv(int socket file descriptor, void *buffer (point to buffer), int size of buffer, int flags);
			the return value is the number of bytes actually read into buffer or -1 if an error ocurred
			OBS: if the return is zero it means that the connection has closed by the other side */
			int bytes_received = recv(socket_execution_thread, buf, MAXDATASIZE-1, 0);
			
			if (bytes_received == -1) {
				perror("recv");
				exit(1);
			}

			printf("Leu\n");
			//buffer[bytes_received] = '\0';
			
			//cJSON *decoded_root = cJSON_Parse(buffer);

			//char *new_encoded_json = cJSON_Print(decoded_root);

			buf[bytes_received] = '\0';

			printf("server: received '%s'\n",buf);

			/* int send(int sockfd, const void *msg, int len, int flags); */

			if (send(socket_execution_thread, "Hello World!", 13, 0) == -1)
				perror("send");
			
			printf("Enviou\n");
			
			close(socket_execution_thread);
			exit(0);
		}
		close(socket_execution_thread);  // parent doesn't need this
	}

	return 0;
}
