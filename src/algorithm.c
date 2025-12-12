#include <stdint.h>
#include <stdio.h>
#include <quadmath.h>

#include "algorithm.h"

/** A high-precision float number, to compute 10+ billion digits. */
typedef __float128 real;

/** The precision of the fractions. */
#define EPSILON ((real)1e-34)

/**
 * @brief Fast modular exponentiation.
 * @param base the base of the exponentiation
 * @param pow the power of the exponentiation
 * @param mod the modulo to apply on each step
 * @return base^pow % mod
 */
static uint64_t pow_mod(uint64_t base, uint64_t pow, const uint64_t mod) {
	uint64_t result;

	if (mod == 1)
		return 0;

	result = 1;
	base %= mod;

	while (pow > 0) {
		if (pow & 1)
			result = (result * base) % mod;

		pow /= 2;
		base = (base * base) % mod;
	}

	return result;
}

/**
 * @brief Computes a single fraction sum.
 * @param n the n-th digit to compute
 * @param m the denominator offset
 * @return the infinite sum for a single term
 */
static real sn(const uint64_t n, const uint8_t m) {
	real sum = 0.0L;
	uint64_t k = 0;

	// We multiply the whole sum by 16^n so as to directly have the decimal
	// part. Thus, the first sum (the most important bits) is with a positive
	// exponent, and the tail sum is with a negative exponent.
	
	// We precompute 1/16 to optimize the tail loop.
	const real inverse_16 = 1.0 / (real)16.0;
	real power = 1.0;

	real numerator;
	uint64_t denominator;
	real term;

	// High-precision part.
	for (; k < n; ++k) {
		denominator = 8 * k + m;
		numerator = (real)pow_mod(16ULL, n - k, denominator);

		sum += numerator / (real)denominator;
	}

	// Low-precision remainder.
	for (;; ++k) {
	   	term = power / (real)((real)8.0 * k + m);

		if (term < EPSILON)
			break;

		power *= inverse_16;
		sum += term;
	}

	return sum;
}

uint8_t pi(const uint64_t n) {
	real digit =
		  4.0 * sn(n, 1)
		- 2.0 * sn(n, 4)
		-		sn(n, 5)
		-		sn(n, 6);

	digit -= floorq(digit);

	return (uint8_t)floorq(digit * (real)16.0);
}
