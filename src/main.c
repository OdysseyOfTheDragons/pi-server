#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <omp.h>

#include "shared.h"
#include "algorithm.h"
#include "converter.h"

#define N 100000

int main(void) {
	uint8_t hex[BLOCK_SIZE * N + 1] = { 0 };

#pragma omp parallel for schedule(static) shared(hex)
	for (uint64_t i = 0; i < N; ++i) {
		uint64_t digits = pi(i);

		for (uint8_t k = 0; k < BYTE; ++k) {
			uint8_t digit = digits >> (((BYTE - 1) - k) * BYTE);
			hex[BLOCK_SIZE * i + 2 * k] = convert_digit((digit >> (BYTE / 2)) & 0xF);
			hex[BLOCK_SIZE * i + 2 * k + 1] = convert_digit(digit & 0xF);
		}
	}
	printf("%s\n", hex);

	/*
	uint8_t *digits = convert(N, hex);
	for (uint64_t i = 0; i < N; i++)
		printf("%d", digits[i]);
	printf("\n");

	free(digits);
	*/
	return 0;
}
