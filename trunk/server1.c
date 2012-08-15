#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

/*
* sequence of socket connection for server 
* int socket(int domain, int type, int protocol);
* int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
* int listen(int sockfd, int backlog);
* int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
*/

int main(void) {
	int port_number, sockfd, newsock; 		/* port number and file descriptors */
	struct sockaddr_in serv_addr, cli_addr;	/* structure containing an internet address */
	socklen_t clilen;					/* address of the client */
	
	port_number = 57531;					/* port communication */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);		/* socket communication using Internet */
	
	bzero((char *)&serv_addr, sizeof(serv_addr));	/* initializes serv_addr to zeros */
	serv_addr.sin_family = AF_INET;			/* family must be AF_INET */
      serv_addr.sin_addr.s_addr = INADDR_ANY;		/* IP address of this server */
      serv_addr.sin_port = htons(port_number);		/* converts a port number in host byte order to a port number in network byte order */
	
	return 0;
}
