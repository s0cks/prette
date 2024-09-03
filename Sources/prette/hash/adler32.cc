#include "prette/hash/adler32.h"

namespace prt {
  static constexpr const uint32_t kModAdler = 65521;
  uint32_t adler32(const uint8_t* bytes, const uword num_bytes) {
    uint32_t a = 1, b = 0;
    for (auto index = 0; index < num_bytes; ++index) {
      a = (a + bytes[index]) % kModAdler;
      b = (b + a) % kModAdler;
    }
    return (b << 16) | a;
  }
}