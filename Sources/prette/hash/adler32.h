#ifndef PRT_ADLER32_H
#define PRT_ADLER32_H

#include <string>
#include <cstring>
#include "prette/platform.h"

namespace prt::adler32 {
  using Digest = uint32_t;

  auto of(const uint8_t* bytes, const uword num_bytes) -> Digest;

  static inline auto
  of(const char* value, const uword length) -> Digest {
    return of((const uint8_t*) value, length); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  }

  static inline auto
  of(const char* value) -> Digest {
    return of(value, strlen(value));
  }

  static inline auto
  of(const std::string& value) -> Digest {
    return of(value.data(), value.length());
  }
}

#endif //PRT_ADLER32_H