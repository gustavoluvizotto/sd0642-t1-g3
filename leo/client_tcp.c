/* exemplo de uso de um cliente que utiliza:
 * Address Domain = Internet Domain
 * Socket Type = Stream Sockets
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "aux_tcp.h"
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
	int sockfd, portno;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];

	if (argc<3)
	{
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	portno = atoi(argv[2]);
	if ((server = gethostbyname(argv[1])) == NULL)
	{
		fprintf(stderr, "ERROR, no such host");
		exit(EXIT_FAILURE);
	}
	if ((sockfd=socket(AF_INET, SOCK_STREAM, 0))<0)
		error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting");

	printf("Enter message: ");
	bzero(buffer, 256);
	fgets(buffer, 255, stdin);
	if ((write(sockfd, buffer, strlen(buffer)))<0)
		error("ERROR writing to socket");
	bzero(buffer, 256);
	if ((read(sockfd, buffer, 255))<0)
		error("ERROR reading from socket");
	printf("%s\n", buffer);

	return 0;
}

