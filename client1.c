#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>     /* used for read and write sys call */
#include <sys/types.h>  /* used for socket sys call*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAXX 10000
#define TAM 125

/*
* int socket(int domain, int type, int protocol);
* int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
* ssize_t read(int fd, void *buf, size_t count);
* ssize_t write(int fd, const void *buf, size_t count);  
*/

int main(void){
        int v[TAM];
        int i, sockfd, port_number;
        char *host;
        struct sockaddr_in serv_addr;
        struct hostent *server;

        port_number = 57531;    /* port communication */
        host = "venon";         /* host who client 1 will communicate */

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
                error("ERROR opening socket");
                exit(1);
        }
        server = gethostbyname(host);
        if (server == NULL) {
                fprintf(stderr,"ERROR, no such host\n");
                exit(0);
        }
/*      serv_addr.sin_family = AF_INET;
        //bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(port_number);        
        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
*/
        bzero(v,TAM);
        srand(time(NULL));
        for (i=0; i<TAM; i++)
                v[i] = rand()%MAXX;     //rand number between 0 to 999

        bzero(v,TAM);
        if (write(sockfd,v,TAM) < 0)
                error("Error writing to socket");

        close(sockfd);
        return 0;
}

