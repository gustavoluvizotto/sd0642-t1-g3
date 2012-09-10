/* Implementação do master
 * Obs: Não fornecer parâmetros na chamada do master */
#include "tp1.h"

/* buffers de recebimento e envio */
struct buffer500 rbuf1 = { { 0 }, { { 0, 0 } } };
struct buffer500 rbuf2 = { { 0 }, { { 0, 0 } } };
struct master_buffer sbuf = { { 0 }, { { 0, 0 } } };

/* argumento para as threads */
struct thread_args {
	int threadNumber;
	int listenPort;
	int sendClientId;
	int sendClientPort;
};

/* protótipos das funções executadas pelas threads */
void *threadRecvPacket(void *);
void *threadSendPacket(void *);

int main() {
	int tid[8]; /* identificação das threads */
	struct thread_args arg[8]; /* para passar argumentos para as threads */
	struct timeval *tsvp; /* apontar para o pedaço do vetor de timestamps que compete a este processo */
	int i; /* variável contadora */

	/* preenchimento dos argumentos para as threads receptoras */
	arg[0].threadNumber = 1;
	arg[0].listenPort = SPORT1;
	arg[1].threadNumber = 2;
	arg[1].listenPort = SPORT2;

	/* chama as threads que receberão os pacotes dos servers de nível 2 */
	pthread_create((pthread_t *) &tid[0], NULL, &threadRecvPacket, (void *) &arg[0]);
	pthread_create((pthread_t *) &tid[1], NULL, &threadRecvPacket, (void *) &arg[1]);
	pthread_join((pthread_t) tid[0], NULL );
	pthread_join((pthread_t) tid[1], NULL );
	/* neste ponto os buffers de recebimento estão preechidos e "timestamped" */

	/******************************************************
	 * 		 								M E R G E												*
	 ******************************************************/

	tsvp = &sbuf.tsv[(15 - 1) * 5];
	gettimeofday(&tsvp[MASTER_MERGE_START], NULL ); /* timestamp antes do merge */
	merge(rbuf1.v, rbuf2.v, sbuf.v, 4 * CTAM, 4 * CTAM); /* merge propriamente dito */
	gettimeofday(&tsvp[MASTER_MERGE_FINISH], NULL ); /*timestamp após o merge */

	/* "merge" dos timestamps dos buffers de recebimento no buffer de envio */
	for (i = 0; i < 82; i++) /* percorre os timestamps dos buffers de recebimento e copia para sbuf somente os que existirem */
		if (rbuf1.tsv[i].tv_sec > 0)
			sbuf.tsv[i] = rbuf1.tsv[i];
		else if (rbuf2.tsv[i].tv_sec > 0)
			sbuf.tsv[i] = rbuf2.tsv[i];

	/* agora falta repassar este buffer para os 8 clients. As threads farão este trabalho. Elas precisam
	 * marcar (timestamp) o buffer imediatamente antes de enviar os pacotes */

	/******************************************************
	 *					 		Envio do buffer para os 8 clients						*
	 ******************************************************/

	/* preenchimento dos argumentos para as threads senders */
	for (i = 0; i < 8; i++) {
		arg[i].threadNumber = arg[i].sendClientId = i + 1; /* começa em 1 */
		arg[i].sendClientPort = CPORT;
	}

	/* chamando as threads senders */
	for (i = 0; i < 8; i++)
		pthread_create((pthread_t *) &tid[i], NULL, &threadSendPacket,
				(void *) &arg[i]);

	for (i = 0; i < 8; i++)
		pthread_join((pthread_t) tid[i], NULL );

	/* após o envio do buffer pros 8 clients, este processo pode morrer... */
	return 0;
}

void *threadRecvPacket(void *vargp) {
	struct thread_args *p = (struct thread_args *) vargp;
	int sockfd;
	int newsockfd;
	struct sockaddr_in local_addr = { 0 }; /* this */
	struct sockaddr_in client_addr = { 0 };
	socklen_t client_addr_len = sizeof(client_addr);
	int n; /* bytes recebidos */
	int aux;
	struct timeval *tsvp =
			p->threadNumber % 2 ? &rbuf1.tsv[14 * 5] : &rbuf2.tsv[14 * 5];

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("ERRO em socket()");

	/* habilita reúso da porta para escuta */
	aux = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &aux, sizeof(int)) < 0)
		error("ERRO em setsockopt()");

	bzero((void *) &local_addr, (size_t) sizeof(local_addr));
	local_addr.sin_port = htons((uint16_t) p->listenPort);
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = INADDR_ANY;

	if ((bind(sockfd, (struct sockaddr *) &local_addr, sizeof(local_addr))) < 0)
		error("ERRO no bind()");
	listen(sockfd, 1);

	if ((newsockfd = accept(sockfd, (struct sockaddr *) &client_addr,
			(socklen_t *) &client_addr_len)) < 0)
		error("ERRO no accept()");

	if (p->threadNumber == 1) {
		n = recv(newsockfd, (void *) &rbuf1, sizeof(struct buffer500), MSG_WAITALL);
		gettimeofday(&tsvp[MASTER_RECV_FINISH_THREAD1], NULL ); /* timestamp logo após recebimento do pacote */
	}
	else {
		n = recv(newsockfd, (void *) &rbuf2, sizeof(struct buffer500), MSG_WAITALL);
		gettimeofday(&tsvp[MASTER_RECV_FINISH_THREAD2], NULL ); /* timestamp logo após recebimento do pacote */
	}
	if (n != sizeof(struct buffer500)) {
		fprintf(stderr, "ERRO em recv(): número incorreto de bytes recebidos\n");
		exit(EXIT_FAILURE);
	}

	close(sockfd);
	close(newsockfd);
	/* a thread já recebeu e marcou o pacote. Pode morrer... */
	return NULL ;
}

void *threadSendPacket(void *vargp) {
	struct thread_args *p = (struct thread_args *) vargp;
	int sockfd;
	struct sockaddr_in server_addr = { 0 };
	struct hostent *server_ent = gethostbyname(
			hostid2hostname(p->sendClientId));
	struct timeval *tsvp = &sbuf.tsv[14 * 5];
	int n; /* bytes enviados */

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("ERRO em socket()");

	bzero((void *) &server_addr, (size_t) sizeof(server_addr));
	if (server_ent == NULL ) {
		fprintf(stderr, "ERROR on gethostbyname()");
		exit(EXIT_FAILURE);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons((uint16_t) p->sendClientPort);
	bcopy((const void *) server_ent->h_addr_list[0],
			(void *) &server_addr.sin_addr.s_addr,
			(size_t) server_ent->h_length);

	/* timestamp logo antes de enviar o pacote pela rede */
	switch (p->sendClientId) {
	case 1:
		gettimeofday(&tsvp[MASTER_SEND_START_CLIENT1], NULL );
		break;
	case 2:
		gettimeofday(&tsvp[MASTER_SEND_START_CLIENT2], NULL );
		break;
	case 3:
		gettimeofday(&tsvp[MASTER_SEND_START_CLIENT3], NULL );
		break;
	case 4:
		gettimeofday(&tsvp[MASTER_SEND_START_CLIENT4], NULL );
		break;
	case 5:
		gettimeofday(&tsvp[MASTER_SEND_START_CLIENT5], NULL );
		break;
	case 6:
		gettimeofday(&tsvp[MASTER_SEND_START_CLIENT6], NULL );
		break;
	case 7:
		gettimeofday(&tsvp[MASTER_SEND_START_CLIENT7], NULL );
		break;
	case 8:
		gettimeofday(&tsvp[MASTER_SEND_START_CLIENT8], NULL );
		break;
	}

	if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr))
			< 0)
		error("ERRO em connect()");
	n = send(sockfd, &sbuf, sizeof(sbuf), 0); /* envia o buffer adiante para o client */
	if (n != sizeof(sbuf)) {
		fprintf(stderr, "ERRO no send(): número incorreto de bytes enviados\n");
		exit(EXIT_FAILURE);
	}
	close(sockfd);
	return NULL ;
}
