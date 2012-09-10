#include "tp1.h"

/* realiza merge dos vetores a[size_a] e
 * b[size_b] em ab[>= size_a + size_b] */
void merge(int a[], int b[], int ab[], int size_a, int size_b) {
	int a_idx = 0; /* índice para percorrer vetor a */
	int b_idx = 0; /* índice para percorrer vetor b */
	int ab_idx = 0; /* índice para percorrer vetor ab */

	while (a_idx < size_a && b_idx < size_b)
		if (a[a_idx] < b[b_idx])
			ab[ab_idx++] = a[a_idx++];
		else
			ab[ab_idx++] = b[b_idx++];
	while (a_idx < size_a)
		ab[ab_idx++] = a[a_idx++];
	while (b_idx < size_b)
		ab[ab_idx++] = b[b_idx++];
}

/* retorna um ponteiro para um vetor (na heap) de tamaho
 * size_a + size_b, que contém o merge dos vetores
 * a[size_a] e b[size_b], passados como parâmetros.  */
int *mergep(int a[], int b[], int size_a, int size_b) {
	int a_idx = 0; /* índice para percorrer vetor a */
	int b_idx = 0; /* índice para percorrer vetor b */
	int ab_idx = 0; /* índice para percorrer vetor ab */
	int *p = (int *) malloc((unsigned int) (size_a + size_b) * sizeof(int));

	while ( a_idx < size_a && b_idx < size_b )
		if (a[a_idx] < b[b_idx])
			p[ab_idx++] = a[a_idx++];
		else
			p[ab_idx++] = b[b_idx++];
	return p;
}
