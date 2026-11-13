#pragma once

#include <cstdint>
#include <vector>

using namespace std;

/** @brief Initial hash value for 64-bit FNV-1a. */
#define FNV_OFFSET_BASIS_64 0xcbf29ce484222325ull

/** @brief Multiplication prime for 64-bit FNV-1a. */
#define FNV_PRIME_64 0x00000100000001b3ull

/**
 * @brief Computes the 64-bit FNV-1a hash of a byte buffer.
 *
 * Supports incremental hashing: pass FNV_OFFSET_BASIS_64 as the first call's
 * intermediate_hash, then feed each call's result into the next call to hash
 * data in chunks. Hashing chunks sequentially yields the same result as
 * hashing the concatenated data in one call.
 *
 * This is a non-cryptographic hash and must not be used for security
 * purposes.
 *
 * @param[in] intermediate_hash Current hash state. Use FNV_OFFSET_BASIS_64
 *            when starting a new hash.
 * @param[in] data Bytes to fold into the hash.
 * @return Updated 64-bit hash state after processing data. Returns
 *         intermediate_hash unchanged if data is empty.
 */
uint64_t fnv1a_hash(uint64_t intermediate_hash, const vector<uint8_t> &data);
