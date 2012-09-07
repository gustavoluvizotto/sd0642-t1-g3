/*
 * tp1.h
 * É o header principal do trabalho prático. Aqui estão presentes:
 *  - includes -> stdio.h, stdlib.h, etc...
 *  - defines -> constantes e macros
 *  - struct definitions
 *  - typedef declarations
 *  - external declarations (extern) para variáveis globais e funções
 */

#ifndef TP1_H_
#define TP1_H_

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h> /* struct sockaddr; socket(); accept(); bind(); connect(); listen(); etc... */
#include <netinet/in.h> /* struct sockaddr_in */
#include <sys/time.h> /* gettimeofday(); struct timeval */
#include <sys/resource.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <errno.h> /* system error numbers */
#include <pthread.h> /* pthread_start() e outras; compilar com -lpthread */
#include <time.h> /* time(2) */
#include <netdb.h>

/********** defines **********/
#define TAM 1000 /* tamanho do vetor principal a ser ordenado */
#define CTAM  TAM/8 /* tamanho do vetor para o programa client */
#define CPORT 21845 /* número da porta de escuta para programa client */
#define SPORT1 21845 /* número da porta de escuta para programa server (thread 1)*/
#define SPORT2 21846 /* número da porta de escuta para programa server (thread 2) */

/* definições para indexar o vetor de timestamps */
#define CLIENT_ORDENATION_START 0
#define CLIENT_ORDENATION_FINISH 1
#define CLIENT_SEND_START 2 /* envio de dados para um servidor */
#define CLIENT_RECV_FINISH 3 /* recebimento de dados do master */
#define SERVER_RECV_FINISH_THREAD1 0 /* recebimento de dados de um client ou server */
#define SERVER_RECV_FINISH_THREAD2 1 /* recebimento de dados de um client ou server */
#define SERVER_MERGE_START 2
#define SERVER_MERGE_FINISH 3
#define SERVER_SEND_START 4 /* envio de dados para um server ou master */
#define MASTER_RECV_FINISH_THREAD1 0 /* recebimento de dados de um server */
#define MASTER_RECV_FINISH_THREAD2 1 /* recebimento de dados de um server */
#define MASTER_MERGE_START 2
#define MASTER_MERGE_FINISH 3
#define MASTER_SEND_CLIENT1 4 /* envio de dados para o client1 */
#define MASTER_SEND_CLIENT2 5 /* envio de dados para o client2 */
#define MASTER_SEND_CLIENT3 6 /* envio de dados para o client3 */
#define MASTER_SEND_CLIENT4 7 /* envio de dados para o client4 */
#define MASTER_SEND_CLIENT5 8 /* envio de dados para o client5 */
#define MASTER_SEND_CLIENT6 9 /* envio de dados para o client6 */
#define MASTER_SEND_CLIENT7 10 /* envio de dados para o client7 */
#define MASTER_SEND_CLIENT8 11 /* envio de dados para o client8 */


/********** enumerações e struct definitions **********/

/* Este é o buffer que será enviado via rede, do client para o server.
 * Contém um vetor v com CTAM inteiros ordenados, e o vetor
 * tsv com os timestamps medidos */
struct client_buffer {
	int v[CTAM];
	struct timeval tsv[82];
};

/* Buffer que o master utiliza para enviar dados para os clientes */
struct master_buffer {
	int v[TAM];
	struct timeval tsv[82];
};

/* Buffer usado pelos servidores de nível 1 para envio do pacote
 * para os servidores do nível 2. Usado também como buffer dos
 * servidores de nível 2, para recebimento dos pacotes enviados
 * pelos servers do nível 1 */
struct buffer250 {
	int v[2*CTAM];
	struct timeval tsv[82];
};

/* Buffer usado pelos servidores de nível 2 para envio ao master,
 * e usado pelo master, para recebimento dos pacotes enviados
 * pelos servers de nível 2 */
struct buffer500 {
	int v[4*CTAM];
	struct timeval tsv[82];
};

/* Obs: A struct do buffer de transferência entre servidores só precisa ser
 * definida no código dos servidores (server.c). Por isso não está declarada
 * aqui. */

/********** typedef declarations **********/

/********** external global variables **********/
extern int v_principal[TAM];

/********** protótipos de funções **********/

extern void error(const char *);
extern void swap(int *, int *);
extern void merge(int *, int *, int *, int, int);
extern void bubble(int *, int);
extern int *mergep(int *, int *, int, int);
extern void gera_numeros_aleatorios(int *, int);
extern char *elapsedtime(const struct timeval, const struct timeval);
extern char *hostid2hostname(int);
extern void showelapsedtimes(const struct timeval *);
#endif /* TP1_H_ */
