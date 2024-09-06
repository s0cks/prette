#include "prette/hash/adler32.h"

namespace prt::adler32 {
  static constexpr const uint32_t kModAdler = 65521;
  static constexpr const uint32_t kAdlerShift = 16;
  
  auto of(const uint8_t* bytes, const uword num_bytes) -> Digest {
    uint32_t a = 1, b = 0;
    for (auto index = 0; index < num_bytes; ++index) {
      a = (a + bytes[index]) % kModAdler;
      b = (b + a) % kModAdler;
    }
    return (b << kAdlerShift) | a;
  }
}