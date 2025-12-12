/**
 * @file
 * The BBP's algorithm implementation.
 */

#pragma once
#include <stdint.h>

/**
 * @brief Computes the n-th digits of $\pi$ in base 16.
 * @param n the offset to the 16-digit block to compute
 * @return a 16-digit block of pi digits
 */
uint64_t pi(const uint64_t n);
