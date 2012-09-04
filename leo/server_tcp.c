/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/resource.h>

void error(const char *msg) { perror(msg); exit(1); }

int main(int argc, char *argv[])
{
	struct rusage r;
	int sockfd, newsockfd;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	if (argc < 2) {
		printf("ERROR, no port provided\n");
		exit(EXIT_FAILURE);
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0)
		error("ERROR opening socket");
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(atoi(argv[1]));	//porta
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	listen(sockfd, 2);
	clilen = sizeof(cli_addr);
	if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0)
		error("ERROR on accept");
	bzero(buffer,256);
	if (read(newsockfd,buffer,255) < 0)
		error("ERROR reading from socket");
	printf("Here is the message: %s\n",buffer);
	if (write(newsockfd,"I got your message",18) < 0)
		error("ERROR writing to socket");
	close(newsockfd);
	close(sockfd);

	getrusage(RUSAGE_SELF, &r);
	printf("User CPU time (s): %f\n", (r.ru_utime.tv_sec * 1000000 + r.ru_utime.tv_usec)/1000000.0);
	printf("System CPU time (s): %f\n", (r.ru_stime.tv_sec * 1000000 + r.ru_stime.tv_usec)/1000000.0);
	printf("maximum resident set size: %ld\n", r.ru_maxrss);
	printf("integral shared memory size: %ld\n", r.ru_ixrss);
	printf("integral unshared data size: %ld\n", r.ru_idrss);
	printf("integral unshared stack size: %ld\n", r.ru_isrss);
	printf("page reclaims (soft page faults): %ld\n", r.ru_minflt);
	printf("page faults (hard page faults): %ld\n", r.ru_majflt);
	printf("swaps: %ld\n", r.ru_nswap);
	printf("block input operations: %ld\n", r.ru_inblock);
	printf("block output operations: %ld\n", r.ru_oublock);
	printf("IPC messages sent: %ld\n", r.ru_msgsnd);
	printf("IPC messages received: %ld\n", r.ru_msgrcv);
	printf("signals received: %ld\n", r.ru_nsignals);
	printf("voluntary context switches: %ld\n", r.ru_nvcsw);
	printf("involuntary context switches: %ld\n", r.ru_nivcsw);

	return 0;
}

