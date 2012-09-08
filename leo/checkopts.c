/*
 * checkopts.c
 *  Description:
 *      Checks if a Socket option is supported, and get its defaults
 *      Pág. 195 do livro UNP.
 *
 *  Created on: Sep 3, 2012
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <netinet/tcp.h>
#include <sys/time.h> /* struct timeval */
#include <sys/socket.h>
#include <unistd.h> /* close(2) */

union val {
	int i_val; /* para flags ou valores inteiros */
	struct linger linger_val;
	struct timeval timeval_val;
} val;

/* o 'static' impede que estas funções
 * sejam acessadas de outro arquivo */
static char* sock_str_flag(union val *, int); /* É uma função que retorna ponteiro para char */
static char* sock_str_int(union val *, int);
static char* sock_str_linger(union val *, int);
static char* sock_str_timeval(union val *, int);

struct sock_opts {
	const char *opt_str;
	int opt_level;
	int opt_name;
	char *(*opt_val_str)(union val *, int);
} sock_opts[] = {
#ifdef SO_BROADCAST
		{ "SO_BROADCAST",				SOL_SOCKET,		SO_BROADCAST,				sock_str_flag },
#else
		{ "SO_BROADCAST",				0,								0,												NULL },
#endif
#ifdef SO_DEBUG
		{ "SO_DEBUG",							SOL_SOCKET,		SO_DEBUG,							sock_str_flag },
#else
		{ "SO_DEBUG",							0,								0,												NULL },
#endif
#ifdef SO_DONTROUTE
		{ "SO_DONTROUTE",				SOL_SOCKET,		SO_DONTROUTE,				sock_str_flag },
#else
		{ "SO_DONTROUTE",				0,								0,												NULL },
#endif
#ifdef SO_ERROR
		{ "SO_ERROR",							SOL_SOCKET,		SO_ERROR,							sock_str_int },
#else
		{ "SO_ERROR",							0,								0,												NULL },
#endif
#ifdef SO_KEEPALIVE
		{ "SO_KEEPALIVE",					SOL_SOCKET,		SO_KEEPALIVE,					sock_str_flag },
#else
		{ "SO_KEEPALIVE",					0,								0,												NULL },
#endif
#ifdef SO_LINGER
		{ "SO_LINGER",						SOL_SOCKET,		SO_LINGER,							sock_str_linger },
#else
		{ "SO_LINGER",						0,								0,												NULL },
#endif
#ifdef SO_OOBINLINE
		{ "SO_OOBINLINE",				SOL_SOCKET,		SO_OOBINLINE,					sock_str_flag },
#else
		{ "SO_OOBINLINE",				0,								0,												NULL },
#endif
#ifdef SO_RCVBUF
		{ "SO_RCVBUF",						SOL_SOCKET,		SO_RCVBUF,							sock_str_int },
#else
		{ "SO_RCVBUF",						0,								0,												NULL },
#endif
#ifdef SO_SNDBUF
		{ "SO_SNDBUF",						SOL_SOCKET,		SO_SNDBUF,						sock_str_int },
#else
		{ "SO_SNDBUF",						0,								0,												NULL },
#endif
#ifdef SO_RCVLOWAT
		{ "SO_RCVLOWAT",					SOL_SOCKET,		SO_RCVLOWAT,					sock_str_int },
#else
		{ "SO_RCVLOWAT",					0,								0,												NULL },
#endif
#ifdef SO_SNDLOWAT
		{ "SO_SNDLOWAT",				SOL_SOCKET,		SO_SNDLOWAT,					sock_str_int},
#else
		{ "SO_SNDLOWAT",				0,								0,												NULL },
#endif
#ifdef SO_RCVTIMEO
		{ "SO_RCVTIMEO",					SOL_SOCKET,		SO_RCVTIMEO,					sock_str_timeval},
#else
		{ "SO_RCVTIMEO",					0,								0,												NULL },
#endif
#ifdef SO_SNDTIMEO
		{ "SO_SNDTIMEO",				SOL_SOCKET,		SO_SNDTIMEO,					sock_str_timeval},
#else
		{ "SO_SNDTIMEO",				0,								0,												NULL },
#endif
#ifdef SO_REUSEADDR
		{ "SO_REUSEADDR",				SOL_SOCKET,		SO_REUSEADDR,					sock_str_flag},
#else
		{ "SO_REUSEADDR",				0,								0,												NULL },
#endif
#ifdef SO_REUSEPORT
		{ "SO_REUSEPORT",				SOL_SOCKET,		SO_REUSEPORT,					sock_str_flag},
#else
		{ "SO_REUSEPORT",				0,								0,												NULL },
#endif
#ifdef SO_TYPE
		{ "SO_TYPE",								SOL_SOCKET,		SO_TYPE,								sock_str_int},
#else
		{ "SO_TYPE",								0,								0,												NULL },
#endif
#ifdef SO_USELOOPBACK
		{ "SO_USELOOPBACK",			SOL_SOCKET,		SO_USELOOPBACK,			sock_str_flag},
#else
		{ "SO_USELOOPBACK",			0,								0,												NULL },
#endif
#ifdef SO_TIMESTAMP
		{ "SO_TIMESTAMP",				SOL_SOCKET,		SO_TIMESTAMP,				sock_str_flag},
#else
		{ "SO_TIMESTAMP",				0,								0,												NULL },
#endif
#ifdef SO_TIMESTAMPING
		{ "SO_TIMESTAMPING",		SOL_SOCKET,		SO_TIMESTAMPING,			sock_str_flag},
#else
		{ "SO_TIMESTAMPING",		0,								0,												NULL },
#endif
#ifdef SO_TIMESTAMPNS
		{ "SO_TIMESTAMPNS",			SOL_SOCKET,		SO_TIMESTAMPNS,			sock_str_flag},
#else
		{ "SO_TIMESTAMPNS",			0,								0,												NULL },
#endif
#ifdef IP_TOS
		{ "IP_TOS",								IPPROTO_IP,		IP_TOS,									sock_str_int},
#else
		{ "IP_TOS",								0,								0,												NULL },
#endif
#ifdef IP_TTL
		{ "IP_TTL",									IPPROTO_IP,		IP_TTL,									sock_str_int},
#else
		{ "IP_TTL",									0,								0,												NULL },
#endif
#ifdef IPV6_DONTFRAG
		{ "IPV6_DONTFRAG",				IPPROTO_IPV6,	IPV6_DONTFRAG,				sock_str_flag },
#else
		{ "IPV6_DONTFRAG",				0,								0,												NULL },
#endif
#ifdef IPV6_UNICAST_HOPS
		{ "IPV6_UNICAST_HOPS",	IPPROTO_IPV6,	IPV6_UNICAST_HOPS,		sock_str_int },
#else
		{ "IPV6_UNICAST_HOPS",	0,								0,												NULL },
#endif
#ifdef IPV6_V6ONLY
		{ "IPV6_V6ONLY",					IPPROTO_IPV6,	IPV6_V6ONLY,						sock_str_flag },
#else
		{ "IPV6_V6ONLY",					0,								0,												NULL },
#endif
#ifdef SCTP_AUTOCLOSE
		{ "SCTP_AUTOCLOSE",			IPPROTO_SCTP,	SCTP_AUTOCLOSE,				sock_str_int },
#else
		{ "SCTP_AUTOCLOSE",			0,								0,												NULL },
#endif
#ifdef SCTP_MAXBURST
		{ "SCTP_MAXBURST",				IPPROTO_SCTP,	SCTP_MAXBURST,				sock_str_ },
#else
		{ "SCTP_MAXBURST",				0,								0,												NULL },
#endif
#ifdef SCTP_MAXSEG
		{ "SCTP_MAXSEG",					IPPROTO_SCTP,	SCTP_MAXSEG,					sock_str_int },
#else
		{ "SCTP_MAXSEG",					0,								0,												NULL },
#endif
#ifdef SCTP_NODELAY
		{ "SCTP_NODELAY",				IPPROTO_SCTP,	SCTP_NODELAY,					sock_str_flag },
#else
		{ "SCTP_NODELAY",				0,								0,												NULL },
#endif
		{ NULL,										0,								0,												NULL }
};

int main(int argc, char **argv) {
	int fd;
	socklen_t len;
	struct sock_opts *ptr;

	for (ptr = sock_opts; ptr->opt_str != NULL; ptr++) {
		printf("%s: ", ptr->opt_str);
		fflush(stdout);
		if (ptr->opt_val_str == NULL)
			printf("(undefined)\n");
		else {
			switch(ptr->opt_level) {
			case SOL_SOCKET:
				fd = socket(AF_INET, SOCK_STREAM, 0);
				break;
#ifdef IPV6
			case IPPROTO_IPV6:
				fd = socket(AF_INET6, SOCK_STREAM, 0);
				break;
#endif
#ifdef IPPROTO_SCTP
			case IPPROTO_SCTP:
				fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
				break;
#endif
			default:
				printf("Can't create fd for level %d\n", ptr->opt_level);
				exit(EXIT_FAILURE);
				break; /* linha inútil */
			}
			len = sizeof(val);
			if (getsockopt(fd, ptr->opt_level, ptr->opt_name, &val, &len) == -1) {
				printf("getsockopt error");
				exit(EXIT_FAILURE);
			}
			else
				printf("default = %s\n", (*(ptr->opt_val_str))(&val, len));
			close(fd);
		}
	}
	return 0;
}

static char strres[128] = {'\0'}; /* 'static' restringe o uso da variável somente dentro deste arquivo */

static char *sock_str_flag(union val *ptr, int len) {
	if (len != sizeof(int))
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)=%d", len, sizeof(int));
	else
		snprintf(strres, sizeof(strres), "%s", (ptr->i_val == 0) ? "off" : "on");
	return (strres);
}

static char *sock_str_int(union val *ptr, int len) {
	if (len != sizeof(int) && len != sizeof(long int))
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)=%d or sizeof(long int)=%d", len, sizeof(int), sizeof(long int));
	else
		snprintf(strres, sizeof(strres), "%d", ptr->i_val);
	return (strres);
}

static char *sock_str_linger(union val *ptr, int len) {
	if (len != sizeof(struct linger))
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(struct linger)=%d", len, sizeof(struct linger));
	else
		snprintf(strres, sizeof(strres), "l_onoff = %d, l_linger = %d", ptr->linger_val.l_onoff, ptr->linger_val.l_linger);
	return (strres);
}

static char *sock_str_timeval(union val *ptr, int len) {
	if (len != sizeof(struct timeval))
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(struct timeval)=%d", len, sizeof(struct timeval));
	else
		snprintf(strres, sizeof(strres), "%ld sec, %ld usec", ptr->timeval_val.tv_sec, ptr->timeval_val.tv_usec);
	return (strres);
}
