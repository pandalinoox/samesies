#include "samesies/fnv1a_hash.hpp"

uint64_t fnv1a_hash(uint64_t intermediate_hash, const vector<uint8_t> &data) {
  for (uint8_t _byte : data) {
    intermediate_hash ^= _byte;
    intermediate_hash *= FNV_PRIME_64;
  }
  return intermediate_hash;
}
