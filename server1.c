#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define TAM 125
/*Bubble sort*/
void bubble(int v[], int qtd) {
	int i, j, aux, k = qtd-1;

	for(i = 0; i < qtd; i++) {
		for(j = 0; j < k; j++) {
			if(v[j] > v[j+1]) {
				aux = v[j];
				v[j] = v[j+1];
				v[j+1]=aux;
			}
		}
		k--;
	}
}
/*
* sequence of socket connection for server 
* int socket(int domain, int type, int protocol);
* int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
* int listen(int sockfd, int backlog);
* int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
*/

int main(void) {
       int port_number, sockfd, newsockfd;		/* port number and file descriptors */
       struct sockaddr_in serv_addr, cli_addr; 		/* structure containing an internet address */
       socklen_t clilen;					/* address of the client */
	int v1[TAM], v2[TAM];				/* vectors sended from clients*/
	int v3[2*TAM];					/* sorted vector */ 
	char *aux1[TAM], *aux2[TAM];			/* receive vectors */        
	int n;							/* if read and write OK */
	int i;
	
       port_number = 57531;                           /* port communication */
       sockfd = socket(AF_INET, SOCK_STREAM, 0);	/* socket communication using Internet */
	 
       bzero((char *)&serv_addr, sizeof(serv_addr));	/* initializes serv_addr to zeros */
       serv_addr.sin_family = AF_INET;			/* family must be AF_INET */
      	serv_addr.sin_addr.s_addr = INADDR_ANY;		/* IP address of this server */
      	serv_addr.sin_port = htons(port_number);	/* converts a port number in host byte order to a port number in network byte order */
        
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) /* binds a socket to an address */
                error("ERROR on binding");
       listen(sockfd,5);	/* process to listen communication. Arg2 is the number of connections that can be waiting while the process is handling a particular connection */
	clilen = sizeof(cli_addr);
	
/*------------COMMUNICATING WITH THE FIRST CLIENT--------------------------------------*/
     	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); /* block process until client connect. Wake up when client is on. Return new socket to communicate */
     	if (newsockfd < 0) 
          	error("ERROR on accept");

	for (i=0; i<TAM; i++) {
		aux1[i] = (char *) malloc (sizeof(char));
		bzero(aux1[i],5);
		n = read(newsockfd,aux1[i],5);
     		if (n < 0) 
			error("ERROR reading from socket");
	}
	for (i=0; i<TAM; i++)
		v1[i] = atoi(aux1[i]);
/*--------------------------------------------------------------------------------------*/

/*------------COMMUNICATING WITH THE SECOND CLIENT--------------------------------------*/
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); /* block process until client connect. Wake up when client is on. Return new socket to communicate */
     	if (newsockfd < 0) 
          	error("ERROR on accept");
	for (i=0; i<TAM; i++) {
		aux2[i] = (char *) malloc (sizeof(char));
		bzero(aux2[i],5);
		n = read(newsockfd,aux2[i],5);
     		if (n < 0) 
			error("ERROR reading from socket");
	}
	for (i=0; i<TAM; i++) 
		v2[i] = atoi(aux2[i]);
/*--------------------------------------------------------------------------------------*/

/*---------- copying the two clients vector to v3 and sort it --------------------------*/
	for (i=0; i<TAM; i++)
		v3[i] = v1[i];
	for (i=TAM; i<2*TAM; i++)
		v3[i] = v2[i-TAM];
	bubble(v3,2*TAM);
	for (i=0; i<2*TAM; i++)
		printf("%d - %d\n",i+1,v3[i]);
/*--------------------------------------------------------------------------------------*/	
	close(newsockfd);
	close(sockfd);
       return 0;
}
