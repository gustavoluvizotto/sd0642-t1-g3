/* Exemplo de uso do getrusage(2) para medidas de tempos
 * e recursos utilizados no processo */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>

int main()
{
	struct rusage r;
	long int i;
	double x = 3234.234;

	for (i=0; i< 10000000; i++)
		x = x*2.1 + i;

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
