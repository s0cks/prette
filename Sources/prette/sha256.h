#ifndef PRT_SHA256_H
#define PRT_SHA256_H

#include <cstdint>
#include <openssl/sha.h>

#include "prette/uint256.h"

namespace prt::sha256 {
  static const uint64_t kDigestSize = SHA256_DIGEST_LENGTH;
  static const uint64_t kSize = kDigestSize;

  static const uint64_t kDefaultNonceSize = 4096;

  uint256 Of(const uint8_t* data, const uint64_t length);
  uint256 Nonce(const uint64_t size = kDefaultNonceSize);
  uint256 Concat(const uint256& lhs, const uint256& rhs);
  uint256 FromHex(const char* data, const uint64_t length);

  static inline uint256
  FromHex(const std::string& data){
    return FromHex(data.data(), data.length());
  }
}

#endif //PRT_SHA256_H