#include "prette/sha256.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <functional>
#include <glog/logging.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/types.h>
#include <ostream>
#include <random>
#include <vector>

#include "prette/platform.h"
#include "prette/uint256.h"
#include "prette/uv/utils.h"

namespace prt::sha256 {
auto Of(const uint8_t* data, const uint64_t length) -> uint256 {
  EVP_MD_CTX* ctx{};
  if ((ctx = EVP_MD_CTX_new()) == nullptr) {
    LOG(ERROR) << "failed to initialize sha256 context.";
    return {};
  }

  EVP_MD* sha256{};
  if ((sha256 = EVP_MD_fetch(nullptr, "SHA256", nullptr)) == nullptr) {
    LOG(ERROR) << "failed to fetch sha256 algorithm.";
    return {};
  }

  if (!EVP_DigestInit_ex(ctx, sha256, nullptr)) {
    LOG(ERROR) << "failed to initialize sha256 digest.";
    return {};
  }

  if (!EVP_DigestUpdate(ctx, data, length)) {
    LOG(ERROR) << "failed to update sha256 digest.";
    return {};
  }

  auto digest = (uint8_t*)OPENSSL_malloc(EVP_MD_get_size(sha256));  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  uint32_t len = 0;
  if (!EVP_DigestFinal_ex(ctx, digest, &len)) {
    LOG(ERROR) << "failed to calculate sha256 digest.";
    return {};
  }

  uint256 hash(digest, len);

  OPENSSL_free(digest);
  EVP_MD_free(sha256);
  EVP_MD_CTX_free(ctx);
  return hash;
}

auto Concat(const uint256& lhs, const uint256& rhs) -> uint256 {
  std::array<uint8_t, kDigestSize * 2> data{};
  memcpy(&data[0], lhs.data(), kDigestSize);
  memcpy(&data[kDigestSize], rhs.data(), kDigestSize);
  return Of(data.cbegin(), kDigestSize * 2);
}

auto FromHex(const char* data, const uint64_t length) -> uint256 {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
  static const uint8_t kHexadecimalHashmap[] = {
      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,  // 01234567
      0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 89:;<=>?
      0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00,  // @ABCDEFG
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // HIJKLMNO
  };

  uint256 hash;
  for (auto pos = 0; ((pos < (hash.size() * 2)) && (pos < length)); pos += 2) {
    uint8_t idx0 = ((uint8_t)data[pos + 0] & 0x1F) ^ 0x10;
    uint8_t idx1 = ((uint8_t)data[pos + 1] & 0x1F) ^ 0x10;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    hash[pos / 2] = (uint8_t)(kHexadecimalHashmap[idx0] << 4) | kHexadecimalHashmap[idx1];
  };
  return hash;
}

auto Nonce(const uint64_t length) -> uint256 {
  using random_bytes_engine = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, uint8_t>;
  static random_bytes_engine kRandomBytesEngine(uv_hrtime());

  std::vector<uint8_t> data(length);
  std::ranges::generate(data, std::ref(kRandomBytesEngine));
  return Of(data.data(), length);
}
}  // namespace prt::sha256