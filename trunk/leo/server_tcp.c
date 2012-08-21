/* exemplo de uso de um servidor que utiliza:
 * Address Domain = Internet Domain
 * Socket Type = Stream Sockets
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>	//internet domain address
#include "aux_tcp.h"
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;	//serv_addr é usado no bind()

	if (argc < 2)
	{
		fprintf(stderr, "ERROR: no port provided\n");
		exit(EXIT_FAILURE);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	if ((sockfd=socket(AF_INET, SOCK_STREAM, 0))<0)
		error("ERROR opening socket");

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	
	listen(sockfd, 2);

	clilen = sizeof(cli_addr);
	if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0)
		error("ERROR on accept");
	
	bzero(buffer, 256);
	if ( (read(newsockfd, buffer, 255)) < 0)
		error("ERROR reading from socket");
	printf("Here is the messsage: %s\n", buffer);
	if ( (write(newsockfd, "I got your message", 18)) < 0 )
		error("ERROR writing to socket");

	return 0;
}

