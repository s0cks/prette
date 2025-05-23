#ifndef PRT_REGION_H
#define PRT_REGION_H

#include <ostream>
#include <units.h>

#include "prette/common.h"
#include "prette/platform.h"

namespace prt {
class Region {
  DEFINE_DEFAULT_COPYABLE_TYPE(Region);

 protected:
  uword start_ = 0;
  uword size_ = 0;

 public:
  Region() = default;
  Region(const uword start, const uword size) :
    start_(start),
    size_(size) {}
  virtual ~Region() = default;

  auto GetStartingAddress() const -> uword {
    return start_;
  }

  auto GetStartingAddressPointer() const -> void* {
    return (void*)GetStartingAddress();  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  }

  auto GetSize() const -> uword {
    return size_;
  }

  auto GetEndingAddress() const -> uword {
    return GetStartingAddress() + GetSize();
  }

  auto GetEndingAddressPointer() const -> void* {
    return (void*)GetEndingAddress();  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  }

  auto Contains(const uword address) const -> bool {
    return GetStartingAddress() <= address && GetEndingAddress() >= address;
  }

  auto Contains(void* ptr) const -> bool {
    return Contains((uword)ptr);  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  }

  auto IsEmpty() const -> bool {
    return GetSize() == 0;
  }

  auto operator==(const Region& rhs) const -> bool {
    return GetStartingAddress() == rhs.GetStartingAddress() && GetSize() == rhs.GetSize();
  }

  auto operator!=(const Region& rhs) const -> bool {
    return GetStartingAddress() != rhs.GetStartingAddress() || GetSize() != rhs.GetSize();
  }

  auto operator<(const Region& rhs) const -> bool {
    if (GetStartingAddress() < rhs.GetStartingAddress())
      return true;
    return GetSize() < rhs.GetSize();
  }

  auto operator>(const Region& rhs) const -> bool {
    if (GetStartingAddress() > rhs.GetStartingAddress())
      return true;
    return GetSize() > rhs.GetSize();
  }

  friend auto operator<<(std::ostream& stream, const Region& rhs) -> std::ostream& {
    using namespace units::data;
    stream << "Region(";
    stream << "start=" << rhs.GetStartingAddressPointer() << ", ";
    stream << "size=" << byte_t(static_cast<double>(rhs.GetSize()));
    stream << ")";
    return stream;
  }

  explicit operator const ubyte*() const {
    return (const ubyte*)GetStartingAddress();  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  }
};
}  // namespace prt

#endif  // PRT_REGION_H