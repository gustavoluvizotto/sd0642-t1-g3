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
        int port_number_receive, port_number_send, sockfd, newsockfd;	/* numero da porta de envio e receb.; file descriptors */
        struct sockaddr_in serv_addr, cli_addr; 			/* estrutura que contem o endereco de internet */
        socklen_t clilen;						/* endereco do cliente */
	int v1[TAM], v2[TAM];						/* vetores recebidos dos clientes */
	int v3[2*TAM];							/* vetor ordenado */ 
	char *aux1[TAM], *aux2[TAM];					/* vetores aux. que recebem dos clientes */        
	int n, i;							/* para verificar se read() e write() deu certo; indice */
	struct hostent *server;         				/* define um host na internet */
	char *host;							/* nome do host */
	char *aux3[2*TAM];						/* vetor auxiliar para envio */
	double tord_i, tord_f, tcom1_i, tcom1_f, tcom2_i, tcom2_f;	/* tempos de ordenacao e de comunicacao, iniciais e finais */
	char tcom1[10], tcom2[10];					/* tempo de comunicacao */
	double tsend_i;							/* tempo de envio */
	char tsend[10];							/* tempo de envio */
				
        port_number_receive = 57533;                    /* porta de comunicacao para recebimento do vetor */
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

	n = read(newsockfd,tcom1,10);
     	if (n < 0) 
		error("ERROR reading from socket");
	for (i=0; i<TAM; i++) {
		aux1[i] = (char *) malloc (5*sizeof(char));
		bzero(aux1[i],5);
		n = read(newsockfd,aux1[i],5);
     		if (n < 0) 
			error("ERROR reading from socket");
	}
	tcom1_f = tempo();
	tcom1_i = strtod(tcom1,NULL);
	close(newsockfd);
	for (i=0; i<TAM; i++)
		v1[i] = atoi(aux1[i]);
/*--------------------------------------------------------------------------------------*/

/*------------Comunicando com o segundo cliente--------------------------------------*/
     	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); /* bloqueia o processo enquanto o cliente se conecta. Acorda quando o cliente esta ativo. Retorna um novo socket para comunicacao */
     	if (newsockfd < 0) 
          	error("ERROR on accept");

        n = read(newsockfd,tcom2,10);
     	if (n < 0) 
		error("ERROR reading from socket");
	for (i=0; i<TAM; i++) {
		aux2[i] = (char *) malloc (5*sizeof(char));
		bzero(aux2[i],5);
		n = read(newsockfd,aux2[i],5);
     		if (n < 0) 
			error("ERROR reading from socket");
	}
	tcom2_f = tempo();
	tcom2_i = strtod(tcom2,NULL);
	close(newsockfd);
	close(sockfd);
	for (i=0; i<TAM; i++) 
		v2[i] = atoi(aux2[i]);
/*--------------------------------------------------------------------------------------*/

/*----------Copiando os dois vetores dos clientes em v3 e ordenando---------------------*/
	for (i=0; i<TAM; i++)
		v3[i] = v1[i];
	for (i=TAM; i<2*TAM; i++)
		v3[i] = v2[i-TAM];
        tord_i = tempo();
	bubble(v3,2*TAM);
        tord_f = tempo();
	for (i=0; i<2*TAM; i++)
		printf("%d - %d\n",i+1,v3[i]);
/*--------------------------------------------------------------------------------------*/
	
/*----------------------enviando o vetor ja ordenado------------------------------------*/
	port_number_send = 57536;
	host = "justiceiro";
	sockfd = socket(AF_INET, SOCK_STREAM, 0);       
        if (sockfd < 0) {
		error("ERROR opening socket");
		exit(1);
        }
	server = gethostbyname(host);
	if (server == NULL) {
              fprintf(stderr,"ERROR, no such host\n");
              exit(0);
        }
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port_number_send);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)       
              error("ERROR connecting");
        tsend_i = tempo();    
 	sprintf(tsend,"%f",tsend_i);
 	n = write(sockfd,tsend,10);
        if (n < 0)    
		error("Error writing to socket");
        for (i=0; i<2*TAM; i++) {   
		aux3[i] = (char *) malloc (5*sizeof(char));
		bzero(aux3[i],5);
		itoa(v3[i],aux3[i],5);		     
		n = write(sockfd,aux3[i],5);	
        	if (n < 0)    
			error("Error writing to socket");
 	}
 	close(sockfd);
 
   	printf("\nTempo de ordenacao: %f\n",tord_f-tord_i);
 	printf("\nTempo de comunicacao com o primeiro cliente: %f\n",tcom1_f-tcom1_i);
 	printf("\nTempo de comunicacao com o segundo  cliente: %f\n",tcom2_f-tcom2_i);
 	
        return 0;
}
