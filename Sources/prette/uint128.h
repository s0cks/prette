#ifndef PRT_UINT128_H
#define PRT_UINT128_H

#include <ostream>
#include <string>

#include "prette/big_number.h"
#include "prette/common.h"

namespace prt {
static constexpr const uword kUInt128Size = 128 / 8;
class uint128 : public BigNumberTemplate<kUInt128Size> {
  DEFINE_DEFAULT_COPYABLE_TYPE(uint128);

 public:
  static inline auto Compare(const uint128& lhs, const uint128& rhs) -> int {
    return memcmp(lhs.data(), rhs.data(), kUInt128Size);
  }

 public:
  uint128() = default;
  uint128(const uint8_t* bytes, const uword num_bytes) :
    BigNumberTemplate(bytes, num_bytes) {}
  uint128(const uint32_t a, const uint32_t b, const uint32_t c, const uint32_t d) :
    uint128() {
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
    const uint32_t data[] = {a, b, c, d};
    CopyFrom((const uint8_t*)data, kSizeInBytes);  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  }
#if defined(ARCH_IS_X64) || defined(ARCH_IS_ARM64)
  uint128(const uint64_t a, const uint64_t b) :
    uint128() {
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
    const uint64_t data[] = {a, b};
    CopyFrom((const uint8_t*)data, kSizeInBytes);  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  }
#endif  // ARCH_IS_X64 || ARCH_IS_ARM64
  ~uint128() override = default;

  auto operator[](const uword idx) -> uword& {
    ASSERT(idx >= 0 && idx <= size());
    return data_[idx];  // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
  }

  auto operator[](const uword idx) const -> uword {
    ASSERT(idx >= 0 && idx <= size());
    return data_[idx];  // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
  }

  auto operator==(const uint128& rhs) const -> bool {
    return Compare(*this, rhs) == 0;
  }

  auto operator!=(const uint128& rhs) const -> bool {
    return Compare(*this, rhs) != 0;
  }

  auto operator<(const uint128& rhs) const -> bool {
    return Compare(*this, rhs) < 0;
  }

  auto operator>(const uint128& rhs) const -> bool {
    return Compare(*this, rhs) > 0;
  }

  explicit operator std::string() const {
    return ToHexString();
  }

  friend auto operator<<(std::ostream& stream, const uint128& rhs) -> std::ostream& {
    return stream << ((const std::string&)rhs);
  }
};
}  // namespace prt

#endif  // PRT_UINT128_H