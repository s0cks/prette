#ifndef PRT_HOUSE_H
#define PRT_HOUSE_H

#include <ostream>
#include <string>
#include <string_view>

#include "prette/platform.h"
#include "prette/relaxed_atomic.h"

namespace prt {
#define FOR_EACH_HOUSING_TYPE(V) \
  V(SingleFamily)                \
  V(Duplex)                      \
  V(LowRise)                     \
  V(HighRise)

enum HousingType : uword {
#define DEFINE_TYPE(Name) k##Name##Housing,
  FOR_EACH_HOUSING_TYPE(DEFINE_TYPE)
#undef DEFINE_TYPE
};

static inline constexpr auto ToString(const HousingType& rhs) -> std::string_view {
  switch (rhs) {
#define DEFINE_TOSTRING(Name)         \
  case HousingType::k##Name##Housing: \
    return #Name;
    FOR_EACH_HOUSING_TYPE(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
    default:
      return "Unknown";
  }
}

static inline auto operator<<(std::ostream& stream, const HousingType& rhs) -> std::ostream& {
  return stream << ToString(rhs);
}

static inline constexpr auto GetMaxNumberOfUnits(const HousingType& rhs) -> uint64_t {
  switch (rhs) {
    case HousingType::kDuplexHousing:
      return 2;
    case HousingType::kLowRiseHousing:
      return 15;
    case HousingType::kHighRiseHousing:
      return 30;
    case HousingType::kSingleFamilyHousing:
    default:
      return 1;
  }
}

struct Housing {
  HousingType type{};
  RelaxedAtomic<uword> available_units = 0;

  inline constexpr auto GetMaxUnits() const -> uint64_t {
    return prt::GetMaxNumberOfUnits(type);
  }

  auto ToString() const -> std::string;

  inline friend auto operator<<(std::ostream& stream, const Housing& rhs) -> std::ostream& {
    return stream << rhs.ToString();
  }
};
}  // namespace prt

#endif  // PRT_HOUSE_H
