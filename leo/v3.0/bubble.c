#include "tp1.h"

/* ordena um vetor v passado como parâmetro, no próprio vetor.
 * size_v: número de elementos no vetor v  */
void bubble(int v[], int size_v) {
	int i, j;

	for (i = 0; i < size_v - 1; i++)
		for (j = size_v - 1; j > i; j--)
			if (v[j - 1] > v[j])
				swap(&v[j - 1], &v[j]);
}
