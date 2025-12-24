#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <omp.h>

#include "shared.h"
#include "algorithm.h"
#include "converter.h"
#include "database.h"

#define N 10

int main(void) {
	printf("Mapping values to enum:\n");
	printf("# SUCCESS\t%d\n", DB_SUCCESS);
	printf("# OPEN_FAIL\t%d\n", DB_OPEN_FAIL);
	printf("# OPEN_WRONG_FORMAT\t%d\n", DB_OPEN_WRONG_FORMAT);
	printf("# MIGRATE_FAIL\t%d\n", DB_MIGRATE_FAIL);
	printf("# READ_NO_UNCOMPUTED\t%d\n", DB_READ_NO_UNCOMPUTED);
	printf("# READ_NO_UNCHECKED\t%d\n", DB_READ_NO_UNCHECKED);
	printf("# READ_OUT_OF_BOUNDS\t%d\n", DB_READ_OUT_OF_BOUNDS);
	printf("# READ_NOT_READY\t%d\n", DB_READ_NOT_READY);
	printf("# WRITE_ALREADY_COMPUTED\t%d\n", DB_WRITE_ALREADY_COMPUTED);
	printf("# WRITE_CHECK_NOT_COMPUTED\t%d\n", DB_WRITE_CHECK_NOT_COMPUTED);
	printf("# WRITE_ALREADY_CHECKED\t%d\n", DB_WRITE_ALREADY_CHECKED);
	printf("# WRITE_OUT_OF_BOUNDS\t%d\n", DB_WRITE_OUT_OF_BOUNDS);

	printf("> Create database\n");
	db_return create = db_create("./database.pidb", 1000);
	if (create.errno != DB_SUCCESS) {
		fprintf(stderr, "[ERROR] (%d) Creation failed\n", create.errno);
		return create.errno;
	}

	printf("> Open database\n");
	db_return open = db_open("./database.pidb");
	if (open.errno != DB_SUCCESS) {
		fprintf(stderr, "[ERROR] (%d) Opening failed\n", open.errno);
		return open.errno;
	}

	database *db = open.value.database;

	uint64_t digits[N] = { 0 };

#pragma omp parallel for schedule(dynamic)
	for (uint64_t i = 0; i < N; ++i)
		digits[i] = pi(i);

	for (uint64_t i = 0; i < N; ++i) {
		db_return write = db_write_computed(db, BLOCK_SIZE * i, digits[i]);

		if (write.errno != DB_SUCCESS)
			printf("> Error: (%ld) %d\n", i, write.errno);
	}

	printf("> Close\n");
	db_close(db);
	return 0;

/*
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

	uint8_t *digits = convert(N, hex);
	for (uint64_t i = 0; i < N; i++)
		printf("%d", digits[i]);
	printf("\n");

	free(digits);
	return 0;
*/
}
