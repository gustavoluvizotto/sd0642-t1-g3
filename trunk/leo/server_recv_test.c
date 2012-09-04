#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>

void error(const char *msg) { perror(msg); exit(EXIT_FAILURE); }

int main(int argc, char **argv) {
	int sockfd, newsockfd, porta = 12340, n;
	socklen_t clilen;
	struct sockaddr_in server_addr, client_addr;
	int opt = 1; /* habilita SO_REUSEADDR e SO_TIMESTAMP */

	char mbuf[200];	/* message buffer */
	unsigned char cmbuf[40] = {0}; /* control message buffer = ancillary data buffer */
	struct iovec myiovec[1] = { {mbuf, sizeof(mbuf)} };
	struct msghdr mymsghdr; /* usado por recvmsg. É esse cara que configura tanto os dados que irão
	pro mbuf quanto pra cmbuf */
	struct timeval tv_antes = {0,0};  /* pegar o tempo de chegada do pacote no programa do usuário */
	struct timeval tv_depois = {0,0}; /* pegar o tempo de chegada do pacote no kernel */
	struct msghdr *msgp = &mymsghdr;	/* assim eu manipulo msgp sem perder a referência da msghdr */
	struct cmsghdr *cmsgp;	/* para manipular a ancillary data */

	/* setando o mymsghdr para pegar o ancillary data do pacote */
	mymsghdr.msg_name = &client_addr;
	mymsghdr.msg_namelen = sizeof(client_addr);
	mymsghdr.msg_iov = myiovec;
	mymsghdr.msg_iovlen = 1;
	mymsghdr.msg_control = cmbuf;
	mymsghdr.msg_controllen = sizeof(cmbuf);
	mymsghdr.msg_flags = 0;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) error("ERROR opening socket");
	opt = 1; setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(porta);
	if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) error ("ERROR binding on port");
	listen(sockfd, 1);
	clilen = sizeof(client_addr);
	if ((newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &clilen)) < 0) error ("ERROR on accept");
	opt = 1; setsockopt(newsockfd, SOL_SOCKET, SO_TIMESTAMP, &opt, sizeof(opt));


	/* recebimento do primeiro pacote */
	bzero(mbuf, 200);
	//	n = recv(newsockfd, mbuf, 199, 0);
	gettimeofday(&tv_antes, NULL);
	n = recvmsg(newsockfd, &mymsghdr, 0);
	if (n < 0) error("ERROR on recv");

//	gettimeofday(&tv_depois, NULL);
//	x = ( tv_depois.tv_sec*1000000 + tv_depois.tv_usec - tv_antes.tv_sec*1000000 - tv_antes.tv_usec )/1000000.0;

	/* pegando o timestamp no ancillary data...
	 * Nesse ponto, o pacote já chegou e o timestamp de chegada está gravado no ancillary data. Peguê-mo-lo...*/
	for (cmsgp = CMSG_FIRSTHDR(msgp); cmsgp; cmsgp = CMSG_NXTHDR(msgp, cmsgp)) {
		if ( (cmsgp->cmsg_level == SOL_SOCKET) && (cmsgp->cmsg_type == SO_TIMESTAMP) ) {
			memcpy(&tv_depois, CMSG_DATA(cmsgp), sizeof(struct timeval));
			printf("Oba! Tem ancillary data pra timestamp!! Iupi!\n");
		}
	}

	printf("[Server] Recebido(s) %d byte(s): %s\n\n", n, mbuf);
	printf("[Server] Timestamp\n");
	printf("Antes: %ld.%06ld s\nDepois: %ld.%06ld s\n", tv_antes.tv_sec, tv_antes.tv_usec, tv_depois.tv_sec, tv_depois.tv_usec);
	bzero(mbuf, 200);
	strcpy(mbuf, "oi, tudo bem?");

	n = send(newsockfd, mbuf, strlen(mbuf), 0);
	if (n < 0) error("ERROR on send");
	close(newsockfd);
	close(sockfd);
	return 0;
}

