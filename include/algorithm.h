/**
 * @file
 * The BBP's algorithm implementation.
 */

#pragma once
#include <stdint.h>

/**
 * @brief Computes the n-th digit of $\pi$ in base 16.
 * @param n the n-th digit to compute
 * @return the n-th hexadecimal digit of $\pi$
 */
uint8_t pi(const uint64_t n);
