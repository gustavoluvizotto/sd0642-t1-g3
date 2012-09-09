#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define TAM 125

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
int main(void) {
        int port_number_receive, sockfd, newsockfd;			/* numero da porta de envio; file descriptors */
        struct sockaddr_in serv_addr, cli_addr; 			/* estrutura que contem o endereco de internet */
        socklen_t clilen;						/* endereco do cliente */
	int v1[4*TAM], v2[4*TAM];					/* vetores recebidos dos clientes */
	int v3[8*TAM];							/* vetor ordenado */ 
	char *aux1[4*TAM], *aux2[4*TAM];				/* vetores aux. que recebem dos clientes */        
	int n, i, j;							/* para verificar se read() e write() deu certo; indice */
	struct hostent *server;         				/* define um host na internet */
	char *host[] = {"x-men", "ciclope", "wolverine", "magneto", 	/* nome dos hosts que serao enviados o vetor final ja ordenado*/
			"hulk", "colossus", "tempestade", "demolidor"};
	int  port_number_send[] = {57538,57538,57539,57539,57540,	/* numero das portas dos clientes descritos em *host[] */
					57540,57541,57541};
	char *aux3[8*TAM];						/* vetor auxiliar para envio */
	double tord_i, tord_f, tcom1_i, tcom1_f;			/* tempos de ordenacao e de comunicacao, iniciais e finais */
	double tcom2_i, tcom2_f;
	char tcom1[10], tcom2[10], tt[10];				/* tempo de comunicacao e total */
	double tsend_i[8];						/* tempo de envio */
	char tsend[8][10];						/* tempo de envio */
	
        port_number_receive = 57537;                   	/* porta de comunicacao para recebimento do vetor */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);	/* comunicacao do socket via internet */
	if (sockfd < 0) {
		error("ERROR opening socket");
		exit(1);
        }
       
        bzero((char *)&serv_addr, sizeof(serv_addr));	/* inicializa serv_addr com zero */
        serv_addr.sin_family = AF_INET;			/* familia deve ser AF_INET */
      	serv_addr.sin_addr.s_addr = INADDR_ANY;		/* endereco IP desse servidor */
      	serv_addr.sin_port = htons(port_number_receive);/* converte uma porta do host para uma porta da internet */
        
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) /* segura o socket para um edereco */
                error("ERROR on binding");
        listen(sockfd,5);	/* comeca a ouvir a comunicacao. Arg2 e o numero de conexoes que podem aguardar enquanto o processo esta manipulando uma conexao particular */
	clilen = sizeof(cli_addr);
	
/*------------Comunicando com o primeiro cliente--------------------------------------*/
     	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); /* bloqueia o processo enquanto o cliente se conecta. Acorda quando o cliente esta ativo. Retorna um novo socket para comunicacao */
     	if (newsockfd < 0) 
          	error("ERROR on accept");

	n = read(newsockfd,tcom1,10);		/* recebe o tempo que o cliente capturou antes de comecar a receber o vetor */
     	if (n < 0) 
		error("ERROR reading from socket");
	for (i=0; i<4*TAM; i++) {		/* recebe o vetor */
		aux1[i] = (char *) malloc (5*sizeof(char));
		bzero(aux1[i],5);
		n = read(newsockfd,aux1[i],5);
     		if (n < 0) 
			error("ERROR reading from socket");
	}
	tcom1_f = tempo();			/* captura o tempo apos receber o vetor todo do servidor principal */
	tcom1_i = strtod(tcom1,NULL);		/* converte o tempo recebido do servidos principal para double */
	close(newsockfd);
	for (i=0; i<4*TAM; i++)
		v1[i] = atoi(aux1[i]);
/*--------------------------------------------------------------------------------------*/

/*------------Comunicando com o segundo cliente--------------------------------------*/
     	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); /* bloqueia o processo enquanto o cliente se conecta. Acorda quando o cliente esta ativo. Retorna um novo socket para comunicacao */
     	if (newsockfd < 0) 
          	error("ERROR on accept");

        n = read(newsockfd,tcom2,10);		/* recebe o tempo que o cliente capturou antes de comecar a receber o vetor */
     	if (n < 0) 
		error("ERROR reading from socket");
	for (i=0; i<4*TAM; i++) {		/* recebe o vetor */
		aux2[i] = (char *) malloc (5*sizeof(char));
		bzero(aux2[i],5);
		n = read(newsockfd,aux2[i],5);
     		if (n < 0) 
			error("ERROR reading from socket");
	}
	tcom2_f = tempo();			/* captura o tempo apos receber o vetor todo do servidor principal */
	tcom2_i = strtod(tcom2,NULL);		/* converte o tempo recebido do servidos principal para double */
	close(newsockfd);
	close(sockfd);
	for (i=0; i<4*TAM; i++) 
		v2[i] = atoi(aux2[i]);
/*--------------------------------------------------------------------------------------*/

/*----------Copiando os dois vetores dos clientes em v3 e ordenando---------------------*/
	for (i=0; i<4*TAM; i++)
		v3[i] = v1[i];
	for (i=4*TAM; i<8*TAM; i++)
		v3[i] = v2[i-4*TAM];
	tord_i = tempo();			/* capturando o tempo de ordenação do vetor */
	bubble(v3,8*TAM);
	tord_f = tempo();
	for (i=0; i<8*TAM; i++)
		printf("%d - %d\n",i+1,v3[i]);
/*--------------------------------------------------------------------------------------*/
	
/*----------------------enviando o vetor ja ordenado------------------------------------*/
        for (j=0; j<8; j++) {					/* envia para cada um dos 8 clientes*/
        	sockfd = socket(AF_INET, SOCK_STREAM, 0);      
        	if (sockfd < 0) {
			error("ERROR opening socket");
			exit(1);
        	}
		server = gethostbyname(host[j]);
		if (server == NULL) {
		      fprintf(stderr,"ERROR, no such host\n");
		      exit(0);
		}
		bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
		serv_addr.sin_port = htons(port_number_send[j]);
		if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)       
		      error("ERROR connecting");
	        tsend_i[j] = tempo();    		/* captura o tempo antes de começar a enviar o vetor */
 		sprintf(tsend[j],"%f",tsend_i[j]);	/* converte para string */
 		n = write(sockfd,tsend[j],10);		/* envia para o servidor esse tempo */
       		if (n < 0)    
			error("Error writing to socket");
		for (i=0; i<8*TAM; i++) {   		/* envia o vetor */
			aux3[i] = (char *) malloc (5*sizeof(char));
			bzero(aux3[i],5);
			itoa(v3[i],aux3[i],5);		       
			n = write(sockfd,aux3[i],5);	
			if (n < 0)    
				error("Error writing to socket");
	 	}
		close(sockfd);
	}
	
        /* impressao dos tempos; final - inicial */
	printf("\nTempo de ordenacao: %f\n",tord_f-tord_i);
 	printf("\nTempo de comunicacao com o primeiro cliente: %f\n",tcom1_f-tcom1_i);
 	printf("\nTempo de comunicacao com o segundo  cliente: %f\n",tcom2_f-tcom2_i);
 	
        return 0;		
}
