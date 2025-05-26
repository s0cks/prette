#ifndef PRT_WORLD_STATE_H
#define PRT_WORLD_STATE_H

#include <ostream>

#include "prette/platform.h"

namespace prt {
#define FOR_EACH_WORLD_STATE(V) \
  V(WorldCreated)               \
  V(WorldDestroyed)

enum WorldState {
#define DEFINE_STATE(Name) k##Name##State,
  FOR_EACH_WORLD_STATE(DEFINE_STATE)
#undef DEFINE_STATE
};

static inline auto operator<<(std::ostream& stream, const WorldState& rhs) -> std::ostream& {
  switch (rhs) {
#define DEFINE_TOSTRING(Name) \
  case k##Name##State:        \
    return stream << #Name;
    FOR_EACH_WORLD_STATE(DEFINE_TOSTRING)
    default:
      return stream << "unknown WorldState: " << static_cast<int64_t>(rhs);
#undef DEFINE_TOSTRING
  }
}
}  // namespace prt

#endif  // PRT_WORLD_STATE_H
