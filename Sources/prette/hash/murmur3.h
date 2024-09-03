#ifndef PRT_MURMUR3_H
#define PRT_MURMUR3_H

#include <string>
#include <cstring>

#include "prette/uint128.h"
#include "prette/platform.h"

namespace prt {
  uint32_t murmur3_32(const uint8_t* bytes, const uword num_bytes);

  static inline uint32_t
  murmur3_32(const char* value, const uword len) {
    return murmur3_32((const uint8_t*) value, len);
  }

  static inline uint32_t
  murmur3_32(const char* value) {
    return murmur3_32(value, strlen(value));
  }

  static inline uint32_t
  murmur3_32(const std::string& value) {
    return murmur3_32(value.data(), value.length());
  }

  uint128 murmur3_128(const uint8_t* bytes, const uword num_bytes);

  static inline uint128
  murmur3_128(const char* value, const uword len) {
    return murmur3_128((const uint8_t*) value, len);
  }

  static inline uint128
  murmur3_128(const char* value) {
    return murmur3_128(value, strlen(value));
  }

  static inline uint128
  murmur3_128(const std::string& value) {
    return murmur3_128(value.data(), value.length());
  }
}

#endif //PRT_MURMUR3_H