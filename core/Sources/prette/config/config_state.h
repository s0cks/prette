#ifndef PRT_CONFIG_STATE_H
#define PRT_CONFIG_STATE_H

#include <ostream>

namespace prt {
#define FOR_EACH_CONFIG_STATE(V) \
  V(Loading)                     \
  V(Loaded)                      \
  V(Generated)                   \
  V(Saving)                      \
  V(Saved)

enum ConfigState {
  kNoConfig = 0,
#define DEFINE_STATE(Name) kConfig##Name,
  FOR_EACH_CONFIG_STATE(DEFINE_STATE)
#undef DEFINE_STATE
      kTotalNumberOfConfigStates,
};

static inline auto operator<<(std::ostream& stream, const ConfigState& rhs) -> std::ostream& {
  switch (rhs) {
#define DEFINE_TOSTRING(Name)      \
  case ConfigState::kConfig##Name: \
    return stream << #Name;
    FOR_EACH_CONFIG_STATE(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
    default:
      return stream << "unknown " << typeid(ConfigState).name() << ": " << rhs;
  }
}
}  // namespace prt

#endif  // PRT_CONFIG_STATE_H
