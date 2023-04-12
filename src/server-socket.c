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
#include "cJSON.h"

#define PORT "3490"  // the port users will be connecting to
# define IP "127.0.0.1" // the hardcoded IP for debugging purposes, will be deleted soon
#define BACKLOG 10	 // how many pending connections queue will hold

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

int main(void)
{
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

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_listener_thread = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(socket_listener_thread, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(socket_listener_thread, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_listener_thread);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(socket_listener_thread, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		socket_execution_thread = accept(socket_listener_thread, (struct sockaddr *)&their_addr, &sin_size);
		if (socket_execution_thread == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            close(socket_listener_thread); // child doesn't need the listener

            printf("Entrou\n");

            char buffer[1024];
            int bytes_received = recv(socket_listener_thread, buffer, sizeof(buffer), 0);
            
            buffer[bytes_received] = '\0';
            
            cJSON *decoded_root = cJSON_Parse(buffer);

            char *new_encoded_json = cJSON_Print(decoded_root);

            printf("%s\n", new_encoded_json);

            // cJSON* name = cJSON_GetObjectItem(decoded_root, "name");
            // printf("Name: %s\n", name->valuestring);
            
            close(socket_execution_thread);
            exit(0);
        }
        close(socket_execution_thread);  // parent doesn't need this
    }

	return 0;
}
