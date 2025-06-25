#ifndef PRT_SEASON_H
#define PRT_SEASON_H

#include <ostream>
#include <string_view>

#include "prette/platform.h"

namespace prt {
#define FOR_EACH_SEASON(V) \
  V(Spring)                \
  V(Summer)                \
  V(Autumn)                \
  V(Winter)

enum Season : uword {
// clang-format off
#define DEFINE_SEASON(Name) k##Name,
  FOR_EACH_SEASON(DEFINE_SEASON)
#undef DEFINE_SEASON
  // clang-format on
  kTotalNumberOfSeasons,
};

static inline constexpr auto GetSeasonName(const Season rhs) -> std::string_view {
  switch (rhs) {
#define DEFINE_TOSTRING(Name) \
  case Season::k##Name:       \
    return #Name;
    FOR_EACH_SEASON(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
    default:
      return "Unknown Season";
  }
}

static inline auto operator<<(std::ostream& stream, const Season& rhs) -> std::ostream& {
  return stream << GetSeasonName(rhs);
}
}  // namespace prt

#endif  // PRT_SEASON_H
