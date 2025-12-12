#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <omp.h>

#include "algorithm.h"
#include "converter.h"

#define N 10000

int main(void) {
	uint8_t hex[16 * N + 1] = { 0 };

#pragma omp parallel for schedule(static) shared(hex)
	for (uint64_t i = 0; i < N; ++i) {
		uint64_t digits = pi(i);

		for (uint8_t k = 0; k < 8; ++k) {
			uint8_t digit = digits >> ((7 - k) * 8);
			hex[16 * i + 2 * k] 	= convert_digit((digit >> 4) & 0xF);
			hex[16 * i + 2 * k + 1] = convert_digit(digit 		 & 0xF);
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
