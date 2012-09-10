/* execução: ./rtt_client <hostname>
 * Esse programa envia um buffer grande e pega a resposta com o timestamp do outro lado.
 * Faz isso 21 vezes, descarta a primeira, e fazer estatística com as 20 restantes
 *
 * Matemática envolvida nesse programa:
 *
 * Quando o rtt_client recebe o pacote de retorno do rtt_server, os timestamps tsArrive e tsArrivedOnServer
 * deveriam ser bem próximos, e relacionados pela fórmula:
 *
 * 	tsArrive - tsArrivedOnServer = Delta_t + Epsilon   , onde:
 *
 * 	tsArrive: timestamp quando o pacote de retorno chegou aqui no client
 * 	tsArrivedOnServer: timestamp imediatamente antes do rtt_server responder com o pacote de retorno
 * 	Delta_t: tempo que o pacote de retorno demora pra chegar do rtt_server até aqui no rtt_client
 * 	Epsilon: erro do ponto de vista deste rtt_client. Se > 0 indica que a máquina onde roda o rtt_client
 * 				está adiantada (no futuro), e vice-versa.
 *
 * Acontece que dá pra descobrir um Delta_t médio e um Epsilon médio também. Cenário para descobri-los:
 *
 * Faço: Host A -> rtt_client e Host B -> rtt_server. Isso gera uma instância da fórmula acima:
 * 	tsArrive1 - tsArrivedOnServer1 = Delta_t1 + Epsilon1
 *
 * Inverto os papéis de A e B:  faço Host A = rtt_server  e  Host B = rtt_client. Outra instância da fórmula será gerada:
 * 	tsArrive2 - tsArrivedOnServer2 = Delta_t2 + Epsilon2
 *
 * Fazendo a aproximação plausível Delta_t1 = Delta_t2, que seria o tempo de tráfego A->B e B->A. Nada mais justo
 * que torná-los iguais (na média).
 *
 * Quanto a Epsilon1 e Epsilon2, um é o negativo do outro. Pq???  Epsilon1 é o erro do ponto de vista do Host A. É
 * o erro entre os relógios em qualquer instante. Ele é o "dessincronismo" entre os relógios de A e B. Se pegarmos
 * a fórmula principal com Delta_t = 0 (situação em que não há transmissão de pacotes e posso medir exatamente
 * esse dessincronismo), obtemos Epsilon = tsArrive - tsArrivedOnServer.  Isso seria o erro do ponto de vista
 * do host rodando o rtt_client. Mede o quanto este host está adiantado em relação ao host rodando o rtt_server.
 * Se Epsilon > 0, está adiantado. Se < 0, adiantado de uma quantidade negativa, ou seja, atrasado em relação ao
 * outro host. Assim, Epsilon1 tem que ser o negativo de Epsilon2, pois medem o quanto um host está adiantado
 * em relação ao outro. Se um está adiantado, o outro está obviamente atrasado. E vice-versa. Podemos então
 * fazer Delta_t1 = Delta_t2 = Delta_t, e Epsilon2 = -Epsilon1. Assim, as 2 instâncias da fórmula principal
 * ficariam reescritas assim:
 *
 * 	tsArrive1 - tsArrivedOnServer1 = Delta_t + Epsilon1
 * 	tsArrive2 - tsArrivedOnServer2 = Delta_t - Epsilon1
 *
 * Magicamente, se somarmos e dividirmos por 2, obteremos Delta_t, e se subtrairmos e dividirmos por 2,
 * obteremos Epsilon1. Esse par de programas explora este fato maravilhoso e ajuda a obter o Epsilon.
 *
 * O que este programa retorna como saída é o Epsilon do host executando rtt_client em relação ao que executa
 * rtt_server.
 *
 * Como usá-lo:
 *
 *Execute no HostB: ./rtt_server
 *Execute no HostA: ./rtt_client HostA
 *
 * A saída será o 'Epsilon' de A em relação a B, ou seja, o quanto A está A-DI-AN-TA-DO em relação ao B. O ideal
 * é que Epsilon seja zero. Mas na prática cada máquina fica alguns milisegundos adiantada/atrasada em relação a outra,
 * mesmo usando NTP.
 * */

#include "tp1.h"
#define VTAM 1000000
#define PORT 4345

struct buffer {
	int v[VTAM];
	struct timeval ts;
};

int main(int argc, char **argv) {
	struct buffer sbuf = { { 0 }, { 0, 0 } };
	struct timeval tsSend;
	struct timeval tsArrive;
	struct timeval tsArrivedOnServer;
	int sockfd;
	struct sockaddr_in server_addr = { 0 };
	struct hostent *server_ent = NULL;
	int n; /* bytes trocados */
	int i; //contador

	/* a conta que deve ser feita é: Epsilon = (mediaIda - mediaVolta)/2.0 */
	double mediaIda; //quando este host está recebendo os pacotes de confirmação do outro host
	double mediaVolta; //quando o outro host (peer) está recebendo os pacotes de confirmação deste host
	double epsilon;
	double sum = 0.0;

	if (argc != 2) {
		fprintf(stderr, "Uso: %s <hostname>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	server_ent = gethostbyname(argv[1]);
	gera_numeros_aleatorios((int *) sbuf.v, VTAM);

	bzero((void *) &server_addr, (size_t) sizeof(server_addr));
	if (server_ent == NULL ) {
		fprintf(stderr, "ERROR on gethostbyname()");
		exit(EXIT_FAILURE);
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("ERRO em socket()");
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	bcopy((const void *) server_ent->h_addr_list[0],
			(void *) &server_addr.sin_addr.s_addr,
			(size_t) server_ent->h_length);

	if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr))
			< 0)
		error("ERRO em connect()");

	for (i = 0; i < 21; i++) {
		gettimeofday(&tsSend, NULL ); /* timestamp antes de enviar. Não estou mais usando isso.
		 Na verdade poderia até remover esse membro da struct */
		sbuf.ts = tsSend;

		n = send(sockfd, &sbuf, sizeof(sbuf), 0); /* envia o buffer adiante para o client */
		if (n != sizeof(sbuf)) {
			fprintf(stderr, "ERRO no send(): # incorreto de bytes enviados\n");
			exit(EXIT_FAILURE);
		}
		n = recv(sockfd, &tsArrivedOnServer, sizeof(tsArrivedOnServer),
				MSG_WAITALL);
		gettimeofday(&tsArrive, NULL );

		if (n != sizeof(tsArrivedOnServer)) {
			fprintf(stderr, "ERRO no recv(): # incorreto de bytes recebidos\n");
			exit(EXIT_FAILURE);
		}

		if (i) //ignora a primeira medição (i == 0) pq ela é sempre mais demorada
			sum += atof(elapsedtime(tsArrive, tsArrivedOnServer));
	}
	mediaIda = sum / 20.0;

	/* agora é a volta. Esse cara responde os pacotes grandes do peer com pacotinhos mínimos contendo
	 * o timestamp local apenas */
	for (i = 0; i < 21; i++) {
		n = recv(sockfd, &sbuf, sizeof(sbuf), MSG_WAITALL);
		if (n != sizeof(sbuf)) {
			fprintf(stderr, "ERRO em recv(): # incorreto de bytes recebidos\n");
			exit(EXIT_FAILURE);
		}
		gettimeofday(&tsArrive, NULL );
		n = send(sockfd, &tsArrive, sizeof(tsArrive), 0);
		if (n != sizeof(tsArrive)) {
			fprintf(stderr, "ERRO em send(): # incorreto de bytes enviados\n");
			exit(EXIT_FAILURE);
		}
	}

	/* agora o rtt_server me manda a média que ele obteve, pra eu poder fazer meus cálculos aqui */
	n = recv(sockfd, &mediaVolta, sizeof(mediaVolta), MSG_WAITALL);
	if (n != sizeof(mediaVolta)) {
		fprintf(stderr, "ERRO em recv(): # incorreto de bytes recebidos\n");
		exit(EXIT_FAILURE);
	}

	/* faz a conta do Epsilon e mostra o resultado na tela
	 * O Epsilon é o quanto este host está ADIANTADO em relação ao peer */
	epsilon = (mediaIda - mediaVolta) / 2.0;
	printf("%11s: % 8.6fs | %8.6fs\n", argv[1], epsilon, (mediaIda+mediaVolta)/2.0);
	close(sockfd);
	return 0;
}
