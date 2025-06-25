#ifndef PRT_HASHCODE_H
#define PRT_HASHCODE_H

#include <compare>
#include <cstddef>
#include <functional>
#include <ios>
#include <ostream>

#include "prette/common.h"

namespace prt {
using RawHashCode = size_t;

static constexpr const RawHashCode kInvalidHashCode = 0x0;

class HashCode {
  DEFINE_DEFAULT_COPYABLE_TYPE(HashCode);

 private:
  RawHashCode raw_;

 public:
  constexpr HashCode(const RawHashCode raw = kInvalidHashCode) :
    raw_(raw) {}
  ~HashCode() = default;

  constexpr auto raw() const -> RawHashCode {
    return raw_;
  }

  friend auto operator<<(std::ostream& stream, const HashCode& rhs) -> std::ostream& {
    std::ios::fmtflags state(stream.flags());
    stream << "0x" << std::hex << std::nouppercase << rhs;
    stream.flags(state);
    return stream;
  }

  constexpr operator RawHashCode() const {
    return raw();
  }

  auto operator=(const RawHashCode& rhs) -> HashCode& {
    raw_ = rhs;
    return *this;
  }

  constexpr auto operator==(const HashCode& rhs) const -> bool {
    return raw() == rhs.raw();
  }

  constexpr auto operator==(const RawHashCode& rhs) const -> bool {
    return raw() == rhs;
  }

  constexpr auto operator!=(const HashCode& rhs) const -> bool {
    return raw() != rhs.raw();
  }

  constexpr auto operator!=(const RawHashCode& rhs) const -> bool {
    return raw() != rhs;
  }

  constexpr auto operator<(const HashCode& rhs) const -> bool {
    return raw() < rhs.raw();
  }

  constexpr auto operator<(const RawHashCode& rhs) const -> bool {
    return raw() < rhs;
  }

  constexpr auto operator<=>(const HashCode& rhs) const -> std::strong_ordering {
    return raw() <=> rhs.raw();
  }

  constexpr auto operator<=>(const RawHashCode& rhs) const -> std::strong_ordering {
    return raw() <=> rhs;
  }

  auto operator^=(const RawHashCode& rhs) -> HashCode& {
    RawHashCode h = rhs;
    h += 0x9e3779b9 + (raw_ << 6) + (raw_ >> 2);
    raw_ ^= h;
    return *this;
  }

  auto operator^=(const HashCode& rhs) -> HashCode& {
    RawHashCode h = rhs;
    h += 0x9e3779b9 + (raw_ << 6) + (raw_ >> 2);
    raw_ ^= h;
    return *this;
  }

  template <typename T>
  inline auto operator^=(const T& rhs) -> HashCode& {
    std::hash<T> h{};
    return operator^=(h(rhs));
  }
};
}  // namespace prt

#endif  // PRT_HASHCODE_H
