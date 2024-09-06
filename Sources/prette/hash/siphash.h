#ifndef PRT_SIPHASH_H
#define PRT_SIPHASH_H

#include "prette/hash/siphash_digest.h"

namespace prt {
  template<const uword KeySize>
  static inline auto
  SipHash(const uint8_t* bytes,
          const uword num_bytes,
          const std::array<uint8_t, KeySize>& key,
          const uword c,
          const uword d) -> uword {
    hash::SipHashDigest<KeySize> digest(key, c, d);
    digest.Update(bytes, num_bytes);
    return digest.Digest();
  }

  template<const uword KeySize>
  static inline auto
  SipHash(const char* data,
          const uword length,
          const std::array<uint8_t, KeySize>& key,
          const uword c,
          const uword d) -> uword {
    hash::SipHashDigest<KeySize> digest(key, c, d);
    digest.Update(data, length);
    return digest.Digest();
  }

  template<const uword KeySize>
  static inline auto
  SipHash(const char* data,
          const std::array<uint8_t, KeySize>& key,
          const uword c,
          const uword d) -> uword {
    hash::SipHashDigest<KeySize> digest(key, c, d);
    digest.Update(data);
    return digest.Digest();
  }

  template<const uword KeySize>
  static inline auto
  SipHash(const std::string& data,
          const std::array<uint8_t, KeySize>& key,
          const uword c,
          const uword d) -> uword {
    hash::SipHashDigest<KeySize> digest(key, c, d);
    digest.Update(data);
    return digest.Digest();
  }

  namespace siphash24 {
    template<const uword KeySize>
    static inline auto
    of(const uint8_t* bytes,
       const uword num_bytes,
       const std::array<uint8_t, KeySize>& key) -> uword {
      return SipHash<KeySize>(bytes, num_bytes, key, 2, 4); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    }

    template<const uword KeySize>
    static inline auto
    of(const char* data,
       const uword length,
       const std::array<uint8_t, KeySize>& key) -> uword {
      return of((const uint8_t*) data, length, key); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    }

    template<const uword KeySize>
    static inline auto
    of(const char* data,
       const std::array<uint8_t, KeySize>& key) -> uword {
      return of(data, strlen(data), key);
    }

    template<const uword KeySize>
    static inline auto
    of(const std::string& data,
       const std::array<uint8_t, KeySize>& key) -> uword {
      return of(data.data(), data.length(), key);
    }
  }

  namespace siphash48 {
    template<const uword KeySize>
    static inline auto
    of(const uint8_t* bytes,
       const uword num_bytes,
       const std::array<uint8_t, KeySize>& key) -> uword {
      return SipHash<KeySize>(bytes, num_bytes, key, 4, 8); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    }

    template<const uword KeySize>
    static inline auto
    of(const char* data,
       const uword length,
       const std::array<uint8_t, KeySize>& key) -> uword {
      return of((const uint8_t*) data, length, key); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    }

    template<const uword KeySize>
    static inline auto
    of(const char* data,
       const std::array<uint8_t, KeySize>& key) -> uword {
      return of(data, strlen(data), key);
    }

    template<const uword KeySize>
    static inline auto
    of(const std::string& data,
       const std::array<uint8_t, KeySize>& key) -> uword {
      return of(data.data(), data.length(), key);
    }
  }
}

#endif //PRT_SIPHASH_H