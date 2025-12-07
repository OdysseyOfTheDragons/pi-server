/**
 * @file
 * @brief Converts the digits from base 16 to base 10.
 */

#pragma once
#include <stdint.h>

/**
 * @brief Converts a base 16 digit to a character.
 * @param digit the base 16 digit
 * @return the character representing the base 16 digit
 */
char convert_digit(const uint8_t digit);

/**
 * @brief Converts an array of base 16 digits to base 10.
 * @param n the number of digits to convert
 * @param digits the base 16 digits
 * @param length to store the length of the returned array
 * @return the array of base 10 digits
 */
uint8_t *convert(const uint64_t n, const uint8_t * const digits);
