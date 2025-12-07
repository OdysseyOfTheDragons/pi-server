/**
 * @file
 * @brief The Bellard's formula implementation.
 */

#include <stdint.h>
#include <stdio.h>

#ifdef HIGH_PRECISION
#include <quadmath.h>

/** A high-precision float number, to compute 10+ billion digits. */
typedef __float128 real;

/** Exponentiation. */
#define pow(a,b) (powq(a,b))

/** Floor. */
#define floor(a) (floorq(a))

#else
#include <math.h>

/** A precise float number, faster than __float128. */
typedef long double real;
#define pow(a,b) (powl(a,b))
#define floor(a) (floorl(a))
#endif

#include "algorithm.h"

/** The precision of the fractions. */
#define EPSILON 1e-17

/** Gets (-1)^n. */
#define sign(n) (n & 1 ? -1.0L : 1.0L)

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
 * @param m the denominator base
 * @param p the denominator offset
 * @return the infinite sum for a single term
 */
static real sn(const uint64_t n, const uint8_t m, const uint8_t p) {
	real sum = 0.0L;
	uint64_t k = 0;

	// We multiply the whole sum by 16^n = 2^4n so as to directly have the
	// decimal part. But because there is a division by 2^6 at the beggining,
	// we need to take this into account.
	// Later, for each k, we decrement power by 10, so as to avoid computing
	// 4 * n - 10 * k - 6 each time. A single substraction is faster.
	// But to do so, we need to add 10 upfront to counterbalance the 10 that
	// is substracted just after.
	int64_t power = (int64_t)(4 * n) - 6 + 10;

	uint64_t denominator;
	real term;

	// High-precision part.
	for (; k <= n; ++k) {
		denominator = m * k + p;

		// To have something like 4n - 6 - 10k.
		power -= 10;
		if (power >= 0)
			term = sign(k) * (real)pow_mod(2ULL, (uint64_t)power, denominator) / (real)denominator;
		else
			term = sign(k) / ((real)denominator * pow(2.0L, (real)(-power)));

		sum += term;
	}

	// Low-precision remainder.
	for (;; ++k) {
		denominator = m * k + p;
		power -= 10;
	   	term = 1 / ((real)denominator * pow(2.0L, (real)(-power)));

		if (term < EPSILON)
			break;

		sum += sign(k) * term;
	}

	return sum;
}

/**
 * @brief Computes the n-th digit of $\pi$ in base 16.
 * @param n the n-th digit to compute
 * @return the n-th hexadecimal digit of $\pi$
 */
uint8_t pi(const uint64_t n) {
	real digit =
		-  32.0 * sn(n,  4, 1)
		- 		  sn(n,  4, 3)
		+ 256.0 * sn(n, 10, 1)
		-  64.0 * sn(n, 10, 3)
		-   4.0 * sn(n, 10, 5)
		-   4.0 * sn(n, 10, 7)
		+ 		  sn(n, 10, 9);

	digit -= floor(digit);

	return (uint8_t)floor(digit * 16.0L);
}
