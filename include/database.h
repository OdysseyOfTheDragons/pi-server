/**
 * @file
 * @brief A database to store the digits of pi.
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>

/** A database instance. */
typedef struct database_t database;

/** The possible returned states when reading or writing. */
typedef enum {
	/// The operation succeeded.
	DB_SUCCESS,

	/// The database cannot open the given database.
	DB_OPEN_FAIL,

	/// The database given isn't one of my databases.
	DB_OPEN_WRONG_FORMAT,

	/// The database cannot migrate the database.
	/// This is possible if the size is too important on a memory-lacking
	/// environment.
	DB_MIGRATE_FAIL,

	/// There is no more block to compute.
	DB_READ_NO_UNCOMPUTED,

	/// There is no more block to check.
	DB_READ_NO_UNCHECKED,

	/// The block to read or check is outside the maximum size of the database.
	DB_READ_OUT_OF_BOUNDS,

	/// The block to read hasn't been computed yet.
	DB_READ_NOT_READY,

	/// The computed block to write has already been computed.
	DB_WRITE_ALREADY_COMPUTED,

	/// The block to check hasn't been computed.
	DB_WRITE_CHECK_NOT_COMPUTED,

	/// The block to check has already been checked.
	DB_WRITE_ALREADY_CHECKED,

	/// The block to write is outside the maximum wize of the database.
	DB_WRITE_OUT_OF_BOUNDS,
} db_error;

/** The returned value of all database functions. */
typedef struct {
	/// Database return error code.
	db_error errno;

	union {
		/// Returned database.
		database *database;

		/// Returned position.
		uint64_t position;

		/// Returned block.
		uint64_t block;

		/// Returned boolean.
		bool boolean;
	} value;
} db_return;

/**
 * @brief Creates a database from scratch.
 * @param path the path where to create the database
 * @param max_digits the maximul number of digits inside the database
 * @return only if the operation succeeded
 */
db_return db_create(const char * const path, const uint64_t max_digits);

/**
 * @brief Opens a database (it must exist).
 * @param path the path to a database
 * @return the database to manipulate
 */
db_return db_open(const char * const path);

/**
 * @brief Closes the database.
 * @param db the database to close
 */
void db_close(database * const db);

/**
 * @brief Migrates the database to have a bigger/smaller capacity.
 * @param db the database to migrate
 * @param size the new number of digits to allow inside the database
 */
db_return db_migrate(database * const db, const uint64_t size);

/**
 * @brief Gets the position of one uncomputed block.
 * @param db the database to query
 * @return the position for one uncomputed block
 */
db_return db_read_uncomputed(database * const db);

/**
 * @brief Gets the position of one unchecked block.
 * @param db the database to query
 * @return the position for one unchecked block
 */
db_return db_read_unchecked(database * const db);

/**
 * @brief Is the block at the given position computed?
 * @param db the database to query
 * @param position the position of the block to query
 * @return whether the block has been computed
 */
db_return db_read_is_computed(database * const db, const uint64_t position);

/**
 * @brief Is the block at the given position checked?
 * @param db the database to query
 * @param position the position of the block to query
 * @return whether the block has been checked
 */
db_return db_read_is_checked(database * const db, const uint64_t position);

/**
 * @brief Reads one block.
 * @param db the database to query
 * @param position the position for the block
 * @return the 16-digit block
 */
db_return db_read(database * const db, const uint64_t position);

/**
 * @brief Sets one computed block.
 * @param db the database to query
 * @param position the position of the block
 * @param digits the 16-digit block
 */
db_return db_write_computed(
		database * const db,
		const uint64_t position,
		const uint64_t digits);

/**
 * @brief Sets one block as checked.
 * @param db the database to query
 * @param position the block position
 */
db_return db_write_checked(database * const db, const uint64_t position);
