#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <omp.h>

#include "algorithm.h"
#include "converter.h"

#define N 30000

int main(void) {
	uint8_t hex[N + 1] = { 0 };

#pragma omp parallel for schedule(static) shared(hex)
	for (uint64_t i = 0; i < N; i++) {
		hex[i] = pi(i);
	}

	uint8_t *digits = convert(N, hex);
	for (uint64_t i = 0; i < N; i++)
		printf("%d", digits[i]);
	printf("\n");

	free(digits);
	return 0;
}
