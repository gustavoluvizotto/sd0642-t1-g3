/* não fornecer parâmetros de entrada
 * Esse programa recebe um buffer grande e responde com o timestamp de chegada do pacote.
 * Faz isso 21 vezes para o rtt_client jogar fora a primeira medida e fazer estatística com as
 * 20 restantes */

#include "tp1.h"
#define VTAM 1000000
#define PORT 4345

struct buffer {
	int v[VTAM];
	struct timeval tv;
};

int main(int argc, char **argv) {
	struct buffer sbuf = { { 0 }, { 0, 0 } };
	struct timeval tsArrive;
	int i; //contador
	int sockfd;
	int newsockfd;
	struct sockaddr_in local_addr = { 0 };
	struct sockaddr_in client_addr = { 0 };
	socklen_t client_addr_len = sizeof(client_addr);
	int n; /* bytes trocados */
	int aux;
	double  mediaVolta;
	double sum = 0.0;
	struct timeval tsArrivedOnClient;

	if (argc != 1) {
		fprintf(stderr, "Não fornecer parâmetros de entrada\n");
		exit(EXIT_FAILURE);
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("ERRO em socket()");

	/* habilita reúso da porta para escuta */
	aux = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &aux, sizeof(int)) < 0)
		error("ERRO em setsockopt()");

	bzero((void *) &local_addr, (size_t) sizeof(local_addr));
	local_addr.sin_port = htons(PORT);
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = INADDR_ANY;

	if ((bind(sockfd, (struct sockaddr *) &local_addr, sizeof(local_addr))) < 0)
		error("ERRO no bind()");
	listen(sockfd, 1);

	if ((newsockfd = accept(sockfd, (struct sockaddr *) &client_addr,
			(socklen_t *) &client_addr_len)) < 0)
		error("ERRO no accept()");

	for (i = 0; i < 21; i++) {
		n = recv(newsockfd, (void *) &sbuf, sizeof(sbuf), MSG_WAITALL);
		if (n != sizeof(sbuf)) {
			fprintf(stderr, "ERRO em recv(): # incorreto de bytes recebidos\n");
			exit(EXIT_FAILURE);
		}
		gettimeofday(&tsArrive, NULL );
		n = send(newsockfd, &tsArrive, sizeof(tsArrive), 0);
		if (n != sizeof(tsArrive)) {
			fprintf(stderr, "ERRO em send(): # incorreto de bytes enviados\n");
			exit(EXIT_FAILURE);
		}
	}

	/* agora é a volta. Esse cara manda pacotões e recebe pacotinhos com timestamps */
	for (i = 0; i < 21; i++) {
		n = send(newsockfd, &sbuf, sizeof(sbuf), 0);
		if (n != sizeof(sbuf)) {
			fprintf(stderr, "ERRO no send(): # incorreto de bytes enviados\n");
			exit(EXIT_FAILURE);
		}
		n = recv(newsockfd, &tsArrivedOnClient, sizeof(tsArrivedOnClient), MSG_WAITALL);
		gettimeofday(&tsArrive, NULL );

		if (n != sizeof(tsArrivedOnClient)) {
			fprintf(stderr, "ERRO no recv(): # incorreto de bytes recebidos\n");
			exit(EXIT_FAILURE);
		}

		if (i) //ignora a primeira medição (i == 0) pq ela é sempre mais demorada
			sum += atof(elapsedtime(tsArrive, tsArrivedOnClient));
	}
	mediaVolta = sum / 20.0;
	/* manda esse cara pro rtt_client */
	n = send(newsockfd, &mediaVolta, sizeof(mediaVolta), 0);
	if (n != sizeof(mediaVolta)) {
		fprintf(stderr, "ERRO no send(): # incorreto de bytes enviados\n");
		exit(EXIT_FAILURE);
	}

	close(newsockfd);
	close(sockfd);
	return 0;
}
