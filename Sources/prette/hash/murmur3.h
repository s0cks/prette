#ifndef PRT_MURMUR3_H
#define PRT_MURMUR3_H

#include <string>
#include <cstring>

#include "prette/uint128.h"
#include "prette/platform.h"

namespace prt {
  namespace murmur3_32 {
    using Digest = uint32_t;

    auto of(const uint8_t* bytes, const uword num_bytes) -> Digest;

    static inline auto
    of(const char* value, const uword len) -> Digest {
      return of((const uint8_t*) value, len); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
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

  namespace murmur3_128 {
    using Digest = uint128;

    auto of(const uint8_t* bytes, const uword num_bytes) -> Digest;

    static inline auto
    of(const char* value, const uword len) -> Digest {
      return of((const uint8_t*) value, len); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
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
}

#endif //PRT_MURMUR3_H