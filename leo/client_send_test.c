/* Teste de envio (send()) */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>	/* bzero() */
#include <string.h>		/* strcpy() */
#include <netdb.h>
#include <sys/ioctl.h>	/* ioctl() */
#include <sys/time.h>	/* struct timeval */

void error(const char *msg) { perror(msg); exit(EXIT_FAILURE); }

int main(int argc, char **argv) {
	int sockfd, portno, n;
	struct sockaddr_in server_addr;
	struct hostent *server_ent;
	char buffer[200];
	int i = 1;

	portno = 12340;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/* habilita SO_TIMESTAMP. Falta pegar o timestamp via recvmsg(2) */
	i = 1; setsockopt(sockfd, SOL_SOCKET, SO_TIMESTAMP, &i, sizeof(int));

	if (sockfd < 0) error("ERROR opening socket");
	server_ent = gethostbyname("localhost");
	if (server_ent == NULL) fprintf(stderr, "ERROR on gethostbyname");
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(portno);
	bcopy(server_ent->h_addr_list[0], &server_addr.sin_addr.s_addr, server_ent->h_length);
	if(connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) error("ERROR on connect");
	bzero(buffer, 200);
	memset(buffer, 'a', 199);
	buffer[198] = 'b';
//	strcpy(buffer, "BLOCO DE TEXTO QUALQUER");
	n = send(sockfd, buffer, strlen(buffer), 0);
	if (n < 0) error("ERROR on send (writing to socket)");
	bzero(buffer, 200);
	n = recv(sockfd, buffer, 199, 0);	/* Não sobrescrevo o '\0' */
	if (n < 0) error("ERROR on recv (reading from socket)");
	printf("[Cliente] Buffer após recv:\n%s\n\n", buffer);
	close(sockfd);
	return 0;
}


/* FALTA: Usar o recvmsg(2) e o cmsg(3) pra pegar os ancilliary data da vida. */
