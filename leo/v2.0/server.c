/* Implementação do server
 * Obs: Não esquecer de fornecer o parâmetro host_id na chamada do server */
#include "tp1.h"

///* cada server recebe 2 conexões de entrada de dados. Essa struct guarda
// * os possíveis 2 'clientes' que se conectam a esta instância do server, além
// * de um índice no vetor de timestamps que será utilizado pela thread para
// * que ela saiba de qual cliente veio o pacote recebido */
//struct clientes {
//		int hostid1;
//		int tsvidx1; /* a thread testa se este índice está setado no tsv. Se sim, significa que o pacote veio do hostid1 */
//		int hostid2;
//		int tsvidx2; /* mesma idéia que tsvidx1. Serve pra thread saber qual o id do host que mandou o pacote */
//} cliente;

/* Buffers onde as threads colocarão os 2 pacotes recebidos via rede.
 * Como este código serve tanto para os servers de nível 1 e nível 2,
 * declaramos todos os buffers que podem ser utilizados. Eles devem
 * ser globais */
struct client_buffer rbuf125_1 = {{0}, {{0,0}}}; /*recv buffer para servidores de nível 1 */
struct client_buffer rbuf125_2 = {{0}, {{0,0}}}; /* recv buffer para servidores de nível 1 */
struct buffer250 rbuf250_1 = {{0}, {{0,0}}}; /* recv buffer para servidores de nível 2 */
struct buffer250 rbuf250_2 = {{0}, {{0,0}}}; /* recv buffer para servidores de nível 2 */

/* estrutura utilizada para passar argumentos para as threads */
struct thread_args {
	int numero_thread; /* número da thread */
	int porta_escuta; /* porta em que a thread deverá escutar conexões */
	int host_id; /* id deste server */
	int server_level; /* se é um server de nível 1 ou 2 */
};

/* protótipos das funções executadas pelas 2 threads */
void *threadRecvPacket(void *);


int main(int argc, char **argv) {
	pthread_t tid1, tid2;
	struct thread_args arg1, arg2; /* argumentos para as threads 1 e 2 */
	int host_id; /* host_id desse server */
	int i; /* auxiliar para contadores */
//	struct timeval tvaux; /* auxiliar para medições de temp*/
	struct buffer250 sbuf250 = {{0},{{0,0}}}; /* send buffer caso o server seja de nível 1 */
	struct buffer500 sbuf500 = {{0},{{0,0}}}; /* send buffer caso o server seja de nível 2 */
	int server_id; /* id do server que este server deve se conectar para passar o vetor adiante */
	int server_port; /* porta aberta no server de nível 2 ou master */
	struct timeval *tsvp;	/* ponteiro para o início do "pedaço que lhe compete do vetor de timestamps". Exemplo:
	 * o server com host_id = 9 ficará com o pedaço 40, contendo 5 posições no vetor de timestamps */

	/* variáveis para abertura do socket cliente */
	int sockfd;
	struct sockaddr_in s_addr;
	struct hostent *s_ent;
	int n; /* número de bytes enviados via rede */




////bloco de código anterior
//	int newsockfd; /* utilizados para referenciar sockets */
//	struct sockaddr_in peeraddr, myaddr;
//	int peeraddr_len;
//	struct hostent *server_ent; /* para obter o endereço do server */
//	int client_port = CPORT; /* porta aberta por este programa client, para receber o vetor completo ordenado */
//	struct client_buffer cbuffer = {{0}, {{0,0}}}; /* buffer utilizado para passar o vetor ordenado para o server */
//	struct master_buffer mbuffer = {{0}, {{0,0}}}; /* buffer para receber dados do master */
//	int aux;
////fim bloco de código anterior





	if (argc != 2) {
		fprintf(stderr, "Uso: %s host_id\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	host_id = atoi(argv[1]);
	server_id = (host_id + 1)/2 + 8;
	if (host_id %2)
		server_port = SPORT1; /* host_id ímpares conectam-se na porta SPORT1  do server de nível 2 ou master */
	else
		server_port = SPORT2; /* host_id paresconectam-se na porta SPORT2 do server de nível 2 ou master */

//	/* preenchendo a 'struct clientes'. É através destes valores que as threads deste server
//	 * (desta insância do server) decidem de quem veio o pacote que receberam */
//	switch(host_id) {
//	case 9:
//		cliente.hostid1 = 1;
//		cliente.hostid2 = 2;
//		cliente.tsvidx1 = 2;
//		cliente.tsvidx2 = 7;
//		break;
//	case 10:
//		cliente.hostid1 = 3;
//		cliente.hostid2 = 4;
//		cliente.tsvidx1 = 12;
//		cliente.tsvidx2 = 17;
//		break;
//	case 11:
//		cliente.hostid1 = 5;
//		cliente.hostid2 = 6;
//		cliente.tsvidx1 = 22;
//		cliente.tsvidx2 = 27;
//		break;
//	case 12:
//		cliente.hostid1 = 7;
//		cliente.hostid2 = 8;
//		cliente.tsvidx1 = 32;
//		cliente.tsvidx2 = 37;
//		break;
//	case 13:
//		cliente.hostid1 = 9;
//		cliente.hostid2 = 10;
//		cliente.tsvidx1 = 44;
//		cliente.tsvidx2 = 49;
//		break;
//	case 14:
//		cliente.hostid1 = 11;
//		cliente.hostid2 = 12;
//		cliente.tsvidx1 = 54;
//		cliente.tsvidx2 = 59;
//		break;
//	}

	arg1.host_id = host_id;
	arg1.numero_thread = 1;
	arg1.porta_escuta = SPORT1;
	arg2.host_id = host_id;
	arg2.numero_thread = 2;
	arg2.porta_escuta = SPORT2;
	if (host_id >= 9 && host_id <= 12)
		arg1.server_level = arg2.server_level = 1;
	else
		arg1.server_level = arg2.server_level = 2;

	/* chama as 2 threads para que elas recebam os pacotes */
	pthread_create(&tid1, NULL, &threadRecvPacket, (void *) &arg1);
	pthread_create(&tid2, NULL, &threadRecvPacket, (void *) &arg2);
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	/* neste ponto os buffers de recebimento estão preenchidos. Agora
	 * falta fazer o merge destes buffers no buffer de envio */

	if (host_id >= 9 && host_id <=12) { /* é um server de nível 1 */
		/* aqui trabalha-se com os seguintes buffers:
		 * Recebimento: rbuf125_1 e rbuf125_2 -> 125 posições cada
		 * Envio:  sbuf250 -> 250 posições */

		tsvp = &sbuf250.tsv[(host_id - 1) * 5]; /* ponteiro para o pedaço do vetor de timestamps que lhe compete */

		/* timestamp antes do merge */
		gettimeofday(&tsvp[SERVER_MERGE_START], NULL);
		merge(rbuf125_1.v, rbuf125_2.v, sbuf250.v, CTAM, CTAM);
		/* timestamp após o merge */
		gettimeofday(&tsvp[SERVER_MERGE_FINISH], NULL);

		/* agora tem que atualizar os timestamps do sbuf250 com os timestamps recebidos nos buffers de recebimento */
		for (i = 0; i < 82; i++) /* percorre os timestamps dos buffers de recebimento e copia para sbuf250 somente os que existirem */
			if (rbuf125_1.tsv[i].tv_sec > 0)
				sbuf250.tsv[i] = rbuf125_1.tsv[i];
			else if (rbuf125_2.tsv[i].tv_sec > 0) /* 1 if() apenas basta pois somente um dos rbuf125_1.tsv[i] ou rbuf125_2.tsv[i] podem estar marcados */
				sbuf250.tsv[i] = rbuf125_2.tsv[i];

		/* neste ponto, o buffer de saída está completo: a atualização de seu timestamp vector e o merge já foram feitos.
		 * Agora este processo deve abrir um SOCKET CLIENT e repassar o pacote para um servidor de nível 2 */

		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) error ("ERRO em socket()");
		bzero((void *) &s_addr, (size_t)  sizeof(s_addr));
		if ((s_ent = gethostbyname(hostid2hostname(server_id))) == NULL) {
			fprintf(stderr, "ERROR on gethostbyname()");
			exit(EXIT_FAILURE);
		}
		s_addr.sin_family = AF_INET;
		s_addr.sin_port = htons((uint16_t) server_port);
		bcopy((const void *) s_ent->h_addr_list[0], (void *) &s_addr.sin_addr.s_addr, (size_t) s_ent->h_length);

		/* marca o buffer com o timestamp atual, logo antes de iniciar a conexão e enviar o pacote */
		gettimeofday(&tsvp[SERVER_SEND_START], NULL);

		if (connect(sockfd, (struct sockaddr *) &s_addr, sizeof(s_addr)) < 0) error("ERRO em connect()");
		n = send(sockfd, &sbuf250, sizeof(sbuf250), 0); /* envia o buffer adiante para o server de nível 2 */
		if (n != sizeof(sbuf250)) error("ERRO on send()");
		close(sockfd);
		/* o servidor de nível 1 já efetuou todas suas tarefas e pode finalizar sua execução */
	}

	if (host_id ==13 || host_id == 14) { /* é um server de nível 2 */
		/* aqui trabalha-se com os seguintes buffers:
		 * Recebimento: rbuf250_1 e rbuf250_2 -> 250 posições cada
		 * Envio:  sbuf500 -> 500 posições */

		tsvp = &sbuf500.tsv[(host_id - 1) * 5]; /* ponteiro para o pedaço do vetor de timestamps que lhe compete */

		/* timestamp antes do merge */
		gettimeofday(&tsvp[SERVER_MERGE_START], NULL);
		merge(rbuf250_1.v, rbuf250_2.v, sbuf500.v, 2*CTAM, 2*CTAM);
		/* timestamp após o merge */
		gettimeofday(&tsvp[SERVER_MERGE_FINISH], NULL);

		/* agora tem que atualizar os timestamps do sbuf500 com os timestamps recebidos nos buffers de recebimento */
		for (i = 0; i < 82; i++) /* percorre os timestamps dos buffers de recebimento e copia para sbuf500 somente os que existirem */
			if (rbuf250_1.tsv[i].tv_sec > 0)
				sbuf500.tsv[i] = rbuf250_1.tsv[i];
			else if (rbuf250_2.tsv[i].tv_sec > 0) /* 1 if() apenas basta pois somente um dos rbuf250_1.tsv[i] ou rbuf250_2.tsv[i] podem estar marcados */
				sbuf500.tsv[i] = rbuf250_2.tsv[i];

		/* neste ponto, o buffer de saída está completo: a atualização de seu timestamp vector e o merge já foram feitos.
		 * Agora este processo deve abrir um socket cliente e repassar o pacote para o master */

		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) error ("ERRO em socket()");
		bzero((void *) &s_addr, (size_t)  sizeof(s_addr));
		if ((s_ent = gethostbyname(hostid2hostname(server_id))) == NULL) {
			fprintf(stderr, "ERROR on gethostbyname()");
			exit(EXIT_FAILURE);
		}
		s_addr.sin_family = AF_INET;
		s_addr.sin_port = htons((uint16_t) server_port);
		bcopy((const void *) s_ent->h_addr_list[0], (void *) &s_addr.sin_addr.s_addr, (size_t) s_ent->h_length);

		/* marca o buffer com o timestamp atual, logo antes de iniciar a conexão e enviar o pacote */
		gettimeofday(&tsvp[SERVER_SEND_START], NULL);

		if (connect(sockfd, (struct sockaddr *) &s_addr, sizeof(s_addr)) < 0) error("ERRO em connect()");
		n = send(sockfd, &sbuf500, sizeof(sbuf500), 0); /* envia o buffer adiante para o master */
		if (n != sizeof(sbuf500)) error("ERRO on send()");
		close(sockfd);
		/* o servidor de nível 2 já efetuou todas suas tarefas e pode finalizar sua execução */
	}
	return 0;
}


/* definição da função da thread */
void *threadRecvPacket(void *vargp) {
	struct thread_args *p = (struct thread_args *) vargp;
	int sockfd;
	int newsockfd;
	struct sockaddr_in local_addr = {0}; /* endereço de socket deste servidor */
	struct sockaddr_in client_addr = {0}; /* endereço de socket do cliente que está conectando neste servidor*/
	socklen_t client_addr_len = sizeof(client_addr);
	int aux;
	int n;
	struct timeval *tsvp;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) error("ERRO em socket()");

	/* habilita reúso da porta SPORT1 para escuta */
	aux = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &aux, sizeof(int)) < 0 ) error("ERRO em setsockopt()");

	bzero((void *) &local_addr, (size_t) sizeof(local_addr));
	local_addr.sin_port = htons((uint16_t) p->porta_escuta);
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = INADDR_ANY;

	if ((bind(sockfd, (struct sockaddr *) &local_addr, sizeof(local_addr))) < 0) error("ERRO no bind()");
	listen(sockfd, 1);

	if ((newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len)) < 0) error("ERRO no accept()");

	/* faz a recepção do pacote e já faz o timestamp dele */
	/* Talvez precise setar a flag MSG_WAITALL, se começar a aparecer lixo no vetor de inteiros */
	if (p->server_level == 1) { /* este processo é um servidor de nível 1 */
		if (p->numero_thread == 1) {
			if ((n = recv(newsockfd, (void *) &rbuf125_1, sizeof(struct client_buffer), 0)) != sizeof(struct client_buffer)) error("ERRO em recv()");
			tsvp = &rbuf125_1.tsv[(p->host_id - 1) * 5]; /* aponta para a base do pedaço que lhe compete do vetor de timestamps do pacote que acabou de receber */
			gettimeofday(&tsvp[SERVER_RECV_FINISH_THREAD1], NULL); /* faz o timestamp */
		}
		else if (p->numero_thread == 2) {
			if ((n = recv(newsockfd, (void *) &rbuf125_2, sizeof(struct client_buffer), 0)) != sizeof(struct client_buffer)) error("ERRO em recv()");
			tsvp = &rbuf125_2.tsv[(p->host_id - 1) * 5]; /* aponta para a base do pedaço que lhe compete do vetor de timestamps do pacote que acabou de receber */
			gettimeofday(&tsvp[SERVER_RECV_FINISH_THREAD2], NULL); /* faz o timestamp */
		}
	}
	else if (p->server_level == 2) { /* este processo é um servidor de nível 2 */
		if (p->numero_thread == 1) {
			if ((n = recv(newsockfd, (void *) &rbuf250_1, sizeof(struct buffer250), 0)) != sizeof(struct buffer250)) error("ERRO em recv()");
			tsvp = &rbuf250_1.tsv[(p->host_id - 1) * 5]; /* aponta para a base do pedaço que lhe compete do vetor de timestamps do pacote que acabou de receber */
			gettimeofday(&tsvp[SERVER_RECV_FINISH_THREAD1], NULL); /* faz o timestamp */
		}
		else if (p->numero_thread == 2) {
			if ((n = recv(newsockfd, (void *) &rbuf250_2, sizeof(struct buffer250), 0)) != sizeof(struct buffer250)) error("ERRO em recv()");
			tsvp = &rbuf250_2.tsv[(p->host_id - 1) * 5]; /* aponta para a base do pedaço que lhe compete do vetor de timestamps do pacote que acabou de receber */
			gettimeofday(&tsvp[SERVER_RECV_FINISH_THREAD2], NULL); /* faz o timestamp */
		}
	}

	/* a thread já recebeu e marcou o pacote. Pode morrer... */
	return NULL;
}