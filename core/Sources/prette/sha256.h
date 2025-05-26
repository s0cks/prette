#ifndef PRT_SHA256_H
#define PRT_SHA256_H

#include <cstdint>
#include <openssl/sha.h>
#include <string>

#include "prette/uint256.h"

namespace prt::sha256 {
static const uint64_t kDigestSize = SHA256_DIGEST_LENGTH;
static const uint64_t kSize = kDigestSize;

static const uint64_t kDefaultNonceSize = 4096;

auto Of(const uint8_t* data, const uint64_t length) -> uint256;
auto Nonce(const uint64_t size = kDefaultNonceSize) -> uint256;
auto Concat(const uint256& lhs, const uint256& rhs) -> uint256;
auto FromHex(const char* data, const uint64_t length) -> uint256;

static inline auto FromHex(const std::string& data) -> uint256 {
  return FromHex(data.data(), data.length());
}

using Digest = uint256;
}  // namespace prt::sha256

#endif  // PRT_SHA256_H