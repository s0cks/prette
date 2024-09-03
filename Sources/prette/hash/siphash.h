#ifndef PRT_SIPHASH_H
#define PRT_SIPHASH_H

#include "prette/hash/siphash_digest.h"

namespace prt {
  template<const uword KeySize>
  static inline uword
  SipHash(const uint8_t* bytes,
          const uword num_bytes,
          const std::array<uint8_t, KeySize>& key,
          const uword c,
          const uword d) {
    hash::SipHashDigest<KeySize> digest(key, c, d);
    digest.Update(bytes, num_bytes);
    return digest.Digest();
  }

  template<const uword KeySize>
  static inline uword
  SipHash(const char* data,
          const uword length,
          const std::array<uint8_t, KeySize>& key,
          const uword c,
          const uword d) {
    hash::SipHashDigest<KeySize> digest(key, c, d);
    digest.Update(data, length);
    return digest.Digest();
  }

  template<const uword KeySize>
  static inline uword
  SipHash(const char* data,
          const std::array<uint8_t, KeySize>& key,
          const uword c,
          const uword d) {
    hash::SipHashDigest<KeySize> digest(key, c, d);
    digest.Update(data);
    return digest.Digest();
  }

  template<const uword KeySize>
  static inline uword
  SipHash(const std::string& data,
          const std::array<uint8_t, KeySize>& key,
          const uword c,
          const uword d) {
    hash::SipHashDigest<KeySize> digest(key, c, d);
    digest.Update(data);
    return digest.Digest();
  }

  template<const uword KeySize>
  static inline uword
  SipHash_2_4(const uint8_t* bytes,
              const uword num_bytes,
              const std::array<uint8_t, KeySize>& key) {
    return SipHash<KeySize>(bytes, num_bytes, key, 2, 4);
  }

  template<const uword KeySize>
  static inline uword
  SipHash_2_4(const char* data,
              const uword length,
              const std::array<uint8_t, KeySize>& key) {
    return SipHash<KeySize>(data, length, key, 2, 4);
  }

  template<const uword KeySize>
  static inline uword
  SipHash_2_4(const char* data,
              const std::array<uint8_t, KeySize>& key) {
    return SipHash<KeySize>(data, key, 2, 4);
  }

  template<const uword KeySize>
  static inline uword
  SipHash_2_4(const std::string& data,
              const std::array<uint8_t, KeySize>& key) {
    return SipHash<KeySize>(data, key, 2, 4);
  }

  template<const uword KeySize>
  static inline uword
  SipHash_4_8(const uint8_t* bytes,
              const uword num_bytes,
              const std::array<uint8_t, KeySize>& key) {
    return SipHash<KeySize>(bytes, num_bytes, key, 4, 8);
  }

  template<const uword KeySize>
  static inline uword
  SipHash_4_8(const char* data,
              const uword length,
              const std::array<uint8_t, KeySize>& key) {
    return SipHash<KeySize>(data, length, key, 4, 8);
  }

  template<const uword KeySize>
  static inline uword
  SipHash_4_8(const char* data,
              const std::array<uint8_t, KeySize>& key) {
    return SipHash<KeySize>(data, key, 4, 8);
  }

  template<const uword KeySize>
  static inline uword
  SipHash_4_8(const std::string& data,
              const std::array<uint8_t, KeySize>& key) {
    return SipHash<KeySize>(data, key, 4, 8);
  }
}

#endif //PRT_SIPHASH_H