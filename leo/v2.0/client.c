/* Implementação do client
 * Obs: Não esquecer de fornecer o parâmetro host_id na chamada do client */
#include "tp1.h"

int main(int argc, char **argv) {
	int sockfd, newsockfd; /* utilizados para referenciar sockets */
	struct sockaddr_in peeraddr, myaddr;
	int peeraddr_len;
	struct hostent *server_ent; /* para obter o endereço do server */
	int server_port = SPORT; /* porta aberta no server */
	int client_port = CPORT; /* porta aberta por este programa client, para receber o vetor completo ordenado */
	int n; /* número de bytes transmitidos (enviados e/ou recebidos) */
	struct client_buffer cbuffer = {{0}, {{0,0}}}; /* buffer utilizado para passar o vetor ordenado para o server */
	struct master_buffer mbuffer = {{0}, {{0,0}}}; /* buffer para receber dados do master */
	int host_id; /* id deste client */
	int aux;

	if (argc != 2) {
		fprintf(stderr, "Uso: %s host_id\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	host_id = atoi(argv[1]);

	/* id do server que este client deve se conectar para passar o vetor adiante.
	 * Por exemplo, se o host_id = 1 ou 2, o server_id será 9. Se o host_id = 11
	 * ou 12, server_id = 14, e assim por diante   */
	int server_id = (host_id + 1)/2 + 8;

	/* ponteiro para o início do "pedaço que lhe compete do vetor de timestamps". Exemplos:
	 * o client com host_id = 1 ficará com o primeiro pedaço, contendo 5 posições no vetor de timestamps;
	 * o client com host_id = 6 ficará com o sexto pedaço, contendo também 5 posições no vetor de timestamps */
	struct timeval *tsvp = &cbuffer.tsv[(host_id - 1) * 5];

	gera_numeros_aleatorios(cbuffer.v,   CTAM);

	/* marcação do tempo (timestamp) antes da ordenação neste client */
	gettimeofday(&tsvp[CLIENT_ORDENATION_START], NULL);
	bubble(cbuffer.v, CTAM);
	/* timestamp após ordenação neste client */
	gettimeofday(&tsvp[CLIENT_ORDENATION_FINISH], NULL);

	/**************************************************
	 *                  ENVIO DO VETOR PARA O SERVER                *
	 **************************************************/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) error("ERRO em socket()");
	if ((server_ent = gethostbyname(hostid2hostname(server_id))) == NULL) {
		fprintf(stderr, "ERRO em gethostbyname(%s)\n", hostid2hostname(server_id));
		exit(EXIT_FAILURE);
	}
	bzero((void *) &peeraddr, (size_t) sizeof(peeraddr));
	peeraddr.sin_family = AF_INET;
	peeraddr.sin_port = htons((uint16_t) server_port);
	bcopy(server_ent->h_addr_list[0], &peeraddr.sin_addr.s_addr, (size_t) server_ent->h_length);

	/* timestamp antes de iniciar a transmissão do vetor ordenado */
	gettimeofday(&tsvp[CLIENT_SEND_START], NULL);
	if (connect(sockfd, (struct sockaddr *) &peeraddr, sizeof(peeraddr)) < 0) error("ERRO em connect()");
	n = send(sockfd, &cbuffer, sizeof(cbuffer), 0);
	if (n != sizeof(cbuffer))
		error("ERRO on send()");
	close(sockfd);

	/* Neste ponto a transferência do buffer foi efetuada.
	 * Falta agora o client abrir um socket servidor e aguardar
	 * o master enviar o vetor completo e ordenado. */


	/**************************************************
	 *               RECEBIMENTO DO VETOR COMPLETO             *
	 **************************************************/

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) error("ERRO em socket()");

	/* habilita reutilização da porta CPORT para escuta, em caso de execuções em sequência */
	aux = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &aux, sizeof(int)) < 0) error("ERRO em setsockopt()");

	bzero((void *) &myaddr, (size_t) sizeof(myaddr));
	myaddr.sin_port = htons((uint16_t) client_port);
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = INADDR_ANY;
	if ((bind(sockfd, (struct sockaddr *) &myaddr, sizeof(myaddr))) < 0) error("ERRO no bind()");
	listen(sockfd, 1);
	peeraddr_len = sizeof(peeraddr);
	if ((newsockfd = accept(sockfd, (struct sockaddr *) &peeraddr, (socklen_t *) &peeraddr_len)) < 0) error("ERRO em accept()");

	/* talvez precise colocar MSG_WAITALL como flag em recv */
	n = recv(newsockfd, &mbuffer, sizeof(mbuffer), 0);
	if (n != sizeof(mbuffer))
		error("ERRO em recv()");

	/* faz tsvp apontar para o novo timestamp vector que acabou de receber do master */
	tsvp = &mbuffer.tsv[(host_id - 1) * 5];

	/* timestamp após recebimento do vetor completo ordenado */
	gettimeofday(&tsvp[CLIENT_RECV_FINISH], NULL);

	/* fecha os sockets pois não precisará mais se comunicar com ninguém */
	close(sockfd);
	close(newsockfd);

	/* Agora este client possui o vetor completo ordenado e o
	 * vetor de timestamps quase completo, pois os tempos de
	 * recebimento do último pacote (do master) ficam com
	 * cada client. Portanto, este client só possui o tempo de
	 * recebimento dele. */

	/**************************************************
	 *                          Imprimindo o vetor na tela                          *
	 **************************************************/
	printf("\n***** VETOR ORDENADO *****\n\n");
	for (aux = 0; aux < TAM; aux++)
		printf("%d  ", mbuffer.v[aux]);
	printf("\n");

	/**************************************************
	 *                          Imprimindo os tempos na tela                      *
	 **************************************************/

	/* Falta agora mostrar os tempos capturados no vetor de timestamps */
	if (host_id == 1) /* mostra os tempos uma única vez, e no host 1 */
		showelapsedtimes(mbuffer.tsv);

	/* todos os hosts client devem mostrar o TTMC (tempo de transmissão master -> client) */
	printf("TMC%d: %s\n", host_id, elapsedtime(tsvp[CLIENT_RECV_FINISH], mbuffer.tsv[73+host_id]));

	return 0;
}
