#ifndef PRT_UINT256_H
#define PRT_UINT256_H

#include "prette/common.h"
#include "prette/big_number.h"

namespace prt {
  class uint256 : public BigNumberTemplate<256> {
  public:
    static inline int
    Compare(const uint256& lhs, const uint256& rhs) {
      return memcmp(lhs.data(), rhs.data(), kSizeInBytes);
    }
  public:
    uint256() = default;
    uint256(const uint8_t* bytes, const uword num_bytes):
      BigNumberTemplate(bytes, num_bytes) {
    }
    uint256(const uint32_t a,
            const uint32_t b,
            const uint32_t c,
            const uint32_t d,
            const uint32_t e,
            const uint32_t f,
            const uint32_t g,
            const uint32_t h):
      uint256() {
      const uint32_t data[] = { a, b, c, d, e, f, g, h };
      CopyFrom((const uint8_t*) &data[0], kSizeInBytes);
    }
#if defined(ARCH_IS_X64) || defined(ARCH_IS_ARM64)
    uint256(const uint64_t a,
            const uint64_t b,
            const uint64_t c,
            const uint64_t d):
      uint256() {
      const uint64_t data[] = { a, b, c, d };
      CopyFrom((const uint8_t*) &data[0], kSizeInBytes);
    }
#endif //ARCH_IS_X64 || ARCH_IS_ARM64
    constexpr uint256(const uint256& rhs):
      BigNumberTemplate() {
      CopyFrom(rhs);
    }
    ~uint256() override = default;

    uword& operator[](const uword idx) {
      PRT_ASSERT(idx >= 0 && idx <= size());
      return data_[idx];
    }

    uword operator[](const uword idx) const {
      PRT_ASSERT(idx >= 0 && idx <= size());
      return data_[idx];
    }

    bool operator==(const uint256& rhs) const {
      return Compare(*this, rhs) == 0;
    }

    bool operator!=(const uint256& rhs) const {
      return Compare(*this, rhs) != 0;
    }

    bool operator<(const uint256& rhs) const {
      return Compare(*this, rhs) < 0;
    }

    bool operator>(const uint256& rhs) const {
      return Compare(*this, rhs) > 0;
    }

    explicit operator std::string() const {
      return ToHexString();
    }

    friend std::ostream& operator<<(std::ostream& stream, const uint256& rhs) {
      return stream << ((const std::string&) rhs);
    }

    void operator=(const uint256& rhs) {
      CopyFrom(rhs);
    }
  };
}

#endif //PRT_UINT256_H