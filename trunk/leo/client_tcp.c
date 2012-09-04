#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/time.h>
#include <sys/resource.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	struct rusage r;
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
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
