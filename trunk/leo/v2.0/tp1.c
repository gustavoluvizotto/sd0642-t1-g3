/* neste arquivo ficam algumas variáveis globais para as 3 versões
 * do programa (client, server e master), bem como as definições de
 * algumas funções auxiliares. */
#include "tp1.h"

/* troca os valores apontados por a e b */
void swap(int *a, int *b) {
	int tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

/* retorna um vetor com n números inteiros gerados aleatoriamente */
void gera_numeros_aleatorios(int *v, int n) {
	int i;

	srand48(time(NULL )); /* inicialização do lrand48() */
	for (i = 0; i < n; i++)
		v[i] = (int) (lrand48() % 10000 - 5000); /* gera números aleatórios */
}

/* retorna uma string contendo o tempo 'tnew - told' em segundos */
char *elapsedtime(const struct timeval tnew, const struct timeval told) {
	char *str = (char *) calloc(20, sizeof(char)); /* calloc() inicializa tudo com 0 */

	snprintf(str, 20, "%.6f",
			(double) (((tnew.tv_sec * 1000000 + tnew.tv_usec) - (told.tv_sec * 1000000 + told.tv_usec)) / 1000000.0));
	return str;
}

/* dado um host_id (int) retorna o hostname (char*) correspondente */
char *hostid2hostname(int host_id) {
	char *str = (char *) calloc(20, sizeof(char));

	switch (host_id) {
	case 1:
		strcpy(str, "x-men");
		break;
	case 2:
		strcpy(str, "ciclope");
		break;
	case 3:
		strcpy(str, "wolverine");
		break;
	case 4:
		strcpy(str, "magneto");
		break;
	case 5:
		strcpy(str, "hulk");
		break;
	case 6:
		strcpy(str, "colossus");
		break;
	case 7:
		strcpy(str, "tempestade");
		break;
	case 8:
		strcpy(str, "demolidor");
		break;
	case 9:
		strcpy(str, "venon");
		break;
	case 10:
		strcpy(str, "vampira");
		break;
	case 11:
		strcpy(str, "gambit");
		break;
	case 12:
		strcpy(str, "elektra");
		break;
	case 13:
		strcpy(str, "anjo");
		break;
	case 14:
		strcpy(str, "justiceiro");
		break;
	case 15:
		strcpy(str, "noturno");
		break;
	}
	return str;
}

/* o parâmetro t aponta para o vetor de timestamps
 * contendo todas as 82 marcas de tempo referentes às trocas de
 * mensagens e ordenações. Porém o tempo TTMCj = TSM_j – TSCj_4,
 * conforme descrito na planilha "timestamp vector.ods", fica disponível
 * para cada processo client. Portanto, esta instância do cliente só tem
 * acesso ao TTMC dele.
 * Obs: o 'const' na declaração me proíbe de alterar o vetor de timestamps
 */
extern void showelapsedtimes(const struct timeval *t) {
	int host_id;
	int server_id;

	printf("\n***** MEDIDAS DE TEMPO *****\n\n");
	printf("Legenda\n");
	printf("TOCi = tempo de ordenação (bubble) do client i\n");
	printf("TOSi = tempo de ordenação (merge) do server i\n");
	printf("TOM = tempo de ordenação (merge) do master i\n");
	printf("TTCiSj = tempo de transmissão client i -> server j\n");
	printf("TTSiSj = tempo de transmissão server i -> server j\n");
	printf("TTSiM = tempo de transmissão server i -> master\n");
	printf("TTMCi = tempo de transmissão master -> client i\n\n");

	/* imprime na tela os 8 TOCi's: tempo de ordenação (bubble) do client i
	 * Os índices em t[] foram ajustados manualmente de acordo com a tabela
	 * no arquivo "timestamp vector.ods"  */
	for (host_id = 1; host_id <= 8; host_id++)
		printf("TOC%d: %s\n", host_id, elapsedtime(t[(host_id-1)*5+1], t[(host_id-1)*5]));

	/* imprime na tela os 6 TOSi's: tempo de ordenação (merge) do server i */
	for (server_id = 9; server_id <= 14; server_id++)
		printf("TOS%d: %s\n", server_id, elapsedtime(t[server_id*5-2], t[server_id*5-3]));

	/* imprime na tela o tempo de ordenação no master: TOM */
	printf("TOM: %s\n", elapsedtime(t[73], t[72]));

	/* imprime na tela o TTCiSj: tempo de transmissão client i -> server j */
	for (host_id = 1; host_id <= 8; host_id++) {
		server_id = (host_id + 1)/2 + 8;
		printf("TTC%dS%d: %s\n", host_id, server_id, elapsedtime(t[(server_id-1)*5+((host_id-1)%2)], t[host_id*5-3]));
	}

	/* imprime na tela o TTSiSj: tempo de transmissão server i -> server j */
	printf("TTS%dS%d: %s\n", 9, 13, elapsedtime(t[60], t[44]));
	printf("TTS%dS%d: %s\n", 10, 13, elapsedtime(t[61], t[49]));
	printf("TTS%dS%d: %s\n", 11, 14, elapsedtime(t[65], t[54]));
	printf("TTS%dS%d: %s\n", 12, 14, elapsedtime(t[66], t[59]));

	/* imprime na tela o TTSiM: tempo de transmissão server i -> master */
	printf("TTS%dM: %s\n", 13, elapsedtime(t[70], t[64]));
	printf("TTS%dM: %s\n", 14, elapsedtime(t[71], t[69]));
}
