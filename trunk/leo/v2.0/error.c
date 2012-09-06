#include "tp1.h"

/* quando uma função gera um erro e seta a variável global errno,
 * esta função imprime o erro e aborta a execução do progrma */
void error(const char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

