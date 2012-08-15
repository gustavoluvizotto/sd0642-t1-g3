#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>     /* used for read and write sys call */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAXX 10000	/* to generate rand numbers betwen 0 to 9999 */
#define TAM 125		/* size of rand vector */

/*
* sequence of socket connection for client
* int socket(int domain, int type, int protocol);
* int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
* ssize_t read(int fd, void *buf, size_t count);
* ssize_t write(int fd, const void *buf, size_t count);  
*/

int main(void){
        int v[TAM];				/* rand vector to future sort */
        int i, sockfd, port_number;		/* index; file descriptor; port number of connection */
        char *host;				/* name of server host */
        struct sockaddr_in serv_addr;	/* structure containing an internet address */
        struct hostent *server;		/* defines a host computer on the Internet */

        port_number = 57531;    /* port communication */
        host = "venon";         /* host who client 1 will communicate */

        sockfd = socket(AF_INET, SOCK_STREAM, 0);	/* socket communication using Internet */
        if (sockfd < 0) {
                error("ERROR opening socket");
                exit(1);
        }
        server = gethostbyname(host);	/* return a pointer to host of a line in the network host data base */
        if (server == NULL) {
                fprintf(stderr,"ERROR, no such host\n");
                exit(0);
        }
//      serv_addr.sin_family = AF_INET;	/* family must be AF_INET */
//      bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length); /* copies length bytes from arg1 to arg2 */
//      serv_addr.sin_port = htons(port_number); /* converts a port number in host byte order to a port number in network byte order */       
//      if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)	/* establish a connection to the server */
//      	error("ERROR connecting");

        bzero(v,TAM);
        srand(time(NULL));
        for (i=0; i<TAM; i++)
                v[i] = rand()%MAXX;     /* rand number between 0 to 9999 */

        //bzero(v,TAM);
        if (write(sockfd,v,TAM) < 0)	/* write vector at the socket */
                error("Error writing to socket");

        close(sockfd);
        return 0;
}

