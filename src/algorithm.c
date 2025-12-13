#include <stdint.h>
#include <string.h>
#include <quadmath.h>

#include "shared.h"
#include "algorithm.h"

/** A high-precision float number, to compute 10+ billion digits. */
typedef __float128 f128;

/** A high-precision integer, to make the modular exponentiation faster. */
typedef unsigned __int128 u128;

/** The precision of the fractions. */
#define EPSILON ((f128)1e-34)

/**
 * @brief Fast modular exponentiation.
 * @param pow the power of the exponentiation
 * @param mod the modulo to apply on each step
 * @return 16^pow % mod
 */
static uint64_t pow_mod(uint64_t pow, const uint64_t mod) {
	u128 result = 1;
	u128 base = 16 % mod;

	while (pow > 0) {
		if (pow & 1)
			result = (result * base) % mod;

		pow /= 2;
		base = (base * base) % mod;
	}

	return (uint64_t)result;
}

/**
 * @brief Computes a single fraction sum.
 * @param n the n-th digit to compute
 * @param m the denominator offset
 * @return the infinite sum for a single term
 */
static f128 sn(const uint64_t n, const uint8_t m) {
	f128 sum = 0.0Q;
	uint64_t k;

	// We multiply the whole sum by 16^n so as to directly have the decimal
	// part. Thus, the first sum (the most important bits) is with a positive
	// exponent, and the tail sum is with a negative exponent.
	
	// We precompute 1/16 to optimize the tail loop.
	const f128 inverse_16 = 1.0Q / 16.0Q;
	f128 power = 1.0Q;

	//f128 numerator;
	//uint64_t denominator;
	f128 term;

	// High-precision part.
	for (k = 0; k < n; ++k) {
		uint64_t denominator = 8.0Q * k + m;
		f128 numerator = (f128)pow_mod(n - k, denominator);

		sum += numerator / (f128)denominator;
	}

	// Low-precision remainder.
	for (k = n;; ++k) {
	   	term = power / (f128)(8.0Q * k + m);

		if (term < EPSILON)
			break;

		power *= inverse_16;
		sum += term;
	}

	return sum;
}

uint64_t pi(const uint64_t n) {
	const uint64_t offset = 16ULL * n;

	f128 digit =
		  4.0Q * sn(offset, 1)
		- 2.0Q * sn(offset, 4)
		-		 sn(offset, 5)
		-		 sn(offset, 6);

	digit -= floorq(digit);

	// Reveal the first hex digit.
	digit *= 16.0Q;

	f128 floor;
	uint64_t ret = 0;
	for (uint8_t k = 0; k < BLOCK_SIZE; ++k) {
		floor = floorq(digit);

		// We want to compile two digits inside a single byte,
		// and the whole 16-digit block must fit inside a uint64_t.
		// So when k = 0, we need to shift by 7 bytes.
		// Also, when k is even, we need to shift 4 more bits to put the digit
		// in the upper bits of the byte.
		const uint8_t shift =
			((BYTE - 1) - (k / 2)) * BYTE + (k & 1 ? 0 : BYTE / 2);
		ret |= (uint64_t)floor << shift;
	
		// Reveal the next hex digit.
		digit = 16.0Q * (digit - floor);
	}

	return ret;
}
