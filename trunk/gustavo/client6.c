#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>     
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <math.h>

#define MAXX 10000      /* usado para gerar os numeros aleatorios */
#define TAM 125         /* tamanho do vetor */

/*Funcao que coleta o tempo*/
double tempo() {
	struct timeval tv;
	gettimeofday(&tv,0);
	return tv.tv_sec + tv.tv_usec/1e6;
}

/*Bubble sort*/
void bubble(int v[], int qtd) {
	int i, j, aux, k = qtd-1;

	for(i = 0; i < qtd; i++) {
		for(j = 0; j < k; j++) {
			if(v[j] > v[j+1]) {
				aux = v[j];
				v[j] = v[j+1];
				v[j+1]=aux;
			}
		}
		k--;
	}
}

/*Funcoes que convertem inteiro para string*/
size_t strlen(const char *string) {
	const char *s;
	s = string;
	while (*s)
		s++;
	return s - string;
}
char *strrev(char *str) {
	char *p1, *p2;
	if (!str || !*str)
		return str;
	for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2) {
		*p1 ^= *p2;
		*p2 ^= *p1;
		*p1 ^= *p2;
	}
	return str;
}
char *itoa(int n, char *s, int b) {
	static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	int i=0, sign;
	    
	if ((sign = n) < 0)
		n = -n;
	do {
		s[i++] = digits[n % b];
	} while ((n /= b) > 0);
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	return strrev(s);
}

/*Inicio do programa*/
int main(void){
        char *v[TAM];                           			/* vetor aleatorio sera ordenado */
        int i, sockfd, newsockfd, port_number_send, port_number_receive;/* indice; file descriptor; numero da porta envio e receb. */
        char *host;                             			/* nome do host */
        struct sockaddr_in serv_addr, cli_addr;				/* estrutura que contem o endereco de internet */
        socklen_t clilen;						/* endereco do cliente (serverp no caso) */
        struct hostent *server;         				/* define um host na internet */
        int n;								/* para verificar se read() e write() deu certo */
	char *aux[8*TAM];						/* usado para receber o vetor de serverp */
	int v2[8*TAM];							/* usado para receber o vetor de serverp */	
	int aux2[TAM];							/* vetor aleatorio gerado que sera ordenado*/
	double tord_i, tord_f, tcom_i, tcom_f;				/* tempos de ordenacao e de comunicacao, iniciais e finais */
	char tcom[10];							/* tempo de comunicacao e total */
	
        port_number_send = 57533;   	/* porta de comunicacao para envio do vetor */
        host = "gambit";         	/* host que o client 5 e 6 irao se comunicar */

        sockfd = socket(AF_INET, SOCK_STREAM, 0);       /* comunicacao do socket via internet */
        if (sockfd < 0) {
		error("ERROR opening socket");
		exit(1);
        }

        server = gethostbyname(host);   /* retorna um ponteiro para o host do cluster */
        if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
        }

      	serv_addr.sin_family = AF_INET; /* a familia precisa ser AF_INET */
      	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length); /* copia length bytes do arg1 para o arg2 */
     	serv_addr.sin_port = htons(port_number_send); /* converte a porta do host para a porta da internet */
      	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)       /* estabelece a conexao com o servidor */
              error("ERROR connecting");

	srand48(50);
        for (i=0; i<TAM; i++)
        	aux2[i] = lrand48()%MAXX;		/* numero aleatorio entre 0 e 9999 */
        tord_i = tempo();
        bubble(aux2,TAM);
        tord_f = tempo();
        
        tcom_i = tempo();
 	sprintf(tcom,"%f",tcom_i);
 	n = write(sockfd,tcom,10);
        if (n < 0)    
		error("Error writing to socket");
        for (i=0; i<TAM; i++) {   
		v[i] = (char *) malloc (5*sizeof(char));
		bzero(v[i],5);
		itoa(aux2[i],v[i],5);	       
		n = write(sockfd,v[i],5);	/* escreve o vetor no socket */
        	if (n < 0)    
			error("Error writing to socket");
 	}

        close(sockfd);
        
/*-----------------recebendo o vetor do servidor principal----------------------------------------------*/
        port_number_receive = 57540;                   /* numero da porta que recebe do servidor principal*/
        sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	if (sockfd < 0) {
		error("ERROR opening socket");
		exit(1);
        }
       
        bzero((char *)&serv_addr, sizeof(serv_addr));	
        serv_addr.sin_family = AF_INET;			
      	serv_addr.sin_addr.s_addr = INADDR_ANY;		
      	serv_addr.sin_port = htons(port_number_receive);
        
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
                error("ERROR on binding");
        listen(sockfd,5);	
	clilen = sizeof(cli_addr);
     	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); 
     	if (newsockfd < 0) 
          	error("ERROR on accept");

	n = read(newsockfd,tcom,10);
     		if (n < 0) 
			error("ERROR reading from socket");
	for (i=0; i<8*TAM; i++) {				/* le o vetor do servidor principal */
		aux[i] = (char *) malloc (5*sizeof(char));
		bzero(aux[i],5);
		n = read(newsockfd,aux[i],5);
     		if (n < 0) 
			error("ERROR reading from socket");
	}
	tcom_f = tempo();
	tcom_i = strtod(tcom,NULL);
	close(sockfd);
        close(newsockfd);
	for (i=0; i<8*TAM; i++) {
		v2[i] = atoi(aux[i]);
		printf("%d - %d\n",i+1,v2[i]);
	}	
        
	printf("\nTempo de ordenacao: %f\n",tord_f-tord_i);
        printf("\nTempo de comunicacao com o servidor principal: %f\n",fabs(tcom_f-tcom_i));

        return 0;
}
