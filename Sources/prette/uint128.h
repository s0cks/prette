#ifndef PRT_UINT128_H
#define PRT_UINT128_H

#include "prette/common.h"
#include "prette/big_number.h"

namespace prt {
  static constexpr const uword kUInt128Size = 128 / 8;
  class uint128 : public BigNumberTemplate<kUInt128Size> {
    DEFINE_DEFAULT_COPYABLE_TYPE(uint128);
  public:
    static inline int
    Compare(const uint128& lhs, const uint128& rhs) {
      return memcmp(lhs.data(), rhs.data(), kUInt128Size);
    }
  public:
    uint128() = default;
    uint128(const uint8_t* bytes, const uword num_bytes):
      BigNumberTemplate(bytes, num_bytes) {
    }
    uint128(const uint32_t a,
            const uint32_t b,
            const uint32_t c,
            const uint32_t d):
      uint128() {
      const uint32_t data[] = { a, b, c, d };
      CopyFrom((const uint8_t*) data, kSizeInBytes);
    }
#if defined(ARCH_IS_X64) || defined(ARCH_IS_ARM64)
    uint128(const uint64_t a,
            const uint64_t b):
      uint128() {
      const uint64_t data[] = { a, b };
      CopyFrom((const uint8_t*) data, kSizeInBytes);
    }
#endif //ARCH_IS_X64 || ARCH_IS_ARM64
    ~uint128() override = default;

    uword& operator[](const uword idx) {
      PRT_ASSERT(idx >= 0 && idx <= size());
      return data_[idx];
    }

    uword operator[](const uword idx) const {
      PRT_ASSERT(idx >= 0 && idx <= size());
      return data_[idx];
    }

    bool operator==(const uint128& rhs) const {
      return Compare(*this, rhs) == 0;
    }

    bool operator!=(const uint128& rhs) const {
      return Compare(*this, rhs) != 0;
    }
    
    bool operator<(const uint128& rhs) const {
      return Compare(*this, rhs) < 0;
    }

    bool operator>(const uint128& rhs) const {
      return Compare(*this, rhs) > 0;
    }

    explicit operator std::string() const {
      return ToHexString();
    }

    friend std::ostream& operator<<(std::ostream& stream, const uint128& rhs) {
      return stream << ((const std::string&) rhs);
    }
  };
}

#endif //PRT_UINT128_H