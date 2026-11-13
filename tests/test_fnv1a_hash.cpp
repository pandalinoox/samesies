#include "samesies/fnv1a_hash.hpp"

#include <criterion/criterion.h>
#include <cstdint>
#include <vector>

Test(fnv1a_hash, empty_input) {
  uint64_t result = fnv1a_hash(FNV_OFFSET_BASIS_64, {});

  cr_assert_eq(result, FNV_OFFSET_BASIS_64);
}

Test(fnv1a_hash, single_byte) {
  std::vector<uint8_t> data = {'a'};

  uint64_t result = fnv1a_hash(FNV_OFFSET_BASIS_64, data);

  cr_assert_eq(result, 0xaf63dc4c8601ec8cULL);
}

Test(fnv1a_hash, multiple_bytes) {
  std::vector<uint8_t> data = {'a', 'b', 'c'};

  uint64_t result = fnv1a_hash(FNV_OFFSET_BASIS_64, data);

  cr_assert_eq(result, 0xe71fa2190541574bULL);
}

Test(fnv1a_hash, incremental_hashing) {
  std::vector<uint8_t> first = {'a', 'b'};
  std::vector<uint8_t> second = {'c'};

  uint64_t intermediate = fnv1a_hash(FNV_OFFSET_BASIS_64, first);

  uint64_t result = fnv1a_hash(intermediate, second);

  cr_assert_eq(result, 0xe71fa2190541574bULL);
}

Test(fnv1a_hash, zero_byte) {
  std::vector<uint8_t> data = {0x00};

  uint64_t result = fnv1a_hash(FNV_OFFSET_BASIS_64, data);

  cr_assert_eq(result, 0xaf63bd4c8601b7dfULL);
}
