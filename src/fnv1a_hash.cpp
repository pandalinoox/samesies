#include "samesies/fnv1a_hash.hpp"

uint64_t fnv1a_hash(uint64_t intermediate_hash, const vector<char> &data) {
  for (char _byte : data) {
    intermediate_hash ^= static_cast<unsigned char>(_byte);
    intermediate_hash *= FNV_PRIME_64;
  }
  return intermediate_hash;
}
