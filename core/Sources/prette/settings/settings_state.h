#ifndef PRT_SETTINGS_STATE_H
#define PRT_SETTINGS_STATE_H

#include <cstdint>
#include <ostream>

namespace prt {
#define FOR_EACH_SETTINGS_STATE(V) \
  V(SettingsInit)                  \
  V(SettingsDeInit)

enum SettingsState : int64_t {
#define DEFINE_STATE(Name) k##Name##State,
  FOR_EACH_SETTINGS_STATE(DEFINE_STATE)
#undef DEFINE_STATE
};

static inline auto operator<<(std::ostream& stream, const SettingsState& rhs) -> std::ostream& {
  switch (rhs) {
#define DEFINE_TOSTRING(Name) \
  case k##Name##State:        \
    return stream << #Name;
    FOR_EACH_SETTINGS_STATE(DEFINE_TOSTRING)
    default:
      return stream << "unknown SettingsState: " << static_cast<int64_t>(rhs);
#undef DEFINE_TOSTRING
  }
}
}  // namespace prt

#endif  // PRT_SETTINGS_STATE_H
