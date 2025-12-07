#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <mpfr.h>

#include "converter.h"

/** To map hexadecimals to their equivalent character. */
const char map[] = "0123456789ABCDEF";

char convert_digit(const uint8_t digit) {
	return map[digit];
}

/**
 * The wanted precision as numbers after the comma. Higher is better but
 * slower. Please note that there is no meaning in computing more digits
 * than the precision here, as they will all amount to 0.
 */
#define PRECISION 33554432

/** The input base. */
#define INPUT_BASE 16.0

/** The output base. */
#define OUTPUT_BASE 10

uint8_t *convert(const uint64_t n, const uint8_t * const digits) {
	mpfr_set_default_prec(PRECISION);

	/** The final pi number. */
	mpfr_t pi;

	/** The $\frac{1}{16^k}$ fraction to convert from base 16 to base 10. */
	mpfr_t ratio;

	/** A single digit. */
	mpfr_t digit;

	uint64_t k = 0;

	// Sets initial value.
	mpfr_init_set_ui(pi, 3ULL, MPFR_RNDD);
	mpfr_init_set_ui(ratio, 1ULL, MPFR_RNDD);
	mpfr_init(digit);

	for (; k < n; ++k) {
		// ratio /= 16
		mpfr_div_ui(ratio, ratio, INPUT_BASE, MPFR_RNDD);

		// Get the k-th digit.
		mpfr_set_ui(digit, digits[k], MPFR_RNDD);

		// digit *= ratio
		mpfr_mul(digit, digit, ratio, MPFR_RNDD);

		// pi += digit
		mpfr_add(pi, pi, digit, MPFR_RNDD);
	}

	// We need to go through a temporary file to get the output in base 10.
	FILE *tmp = tmpfile();
	mpfr_out_str(tmp, OUTPUT_BASE, n, pi, MPFR_RNDD);
	fseek(tmp, 0, SEEK_SET);

	// We now load the file content in our output array.
	uint8_t *output = (uint8_t *)calloc(n, sizeof(uint8_t));

	// We know the first two characters are the integer part (0) and the comma.
	fgetc(tmp); fgetc(tmp);
	for (k = 0; k < n; ++k)
		output[k] = fgetc(tmp) - '0';

	fclose(tmp);

	// Cleanup.
	mpfr_clear(pi);
	mpfr_clear(ratio);
	mpfr_clear(digit);
	mpfr_free_cache();

	return output;
}
