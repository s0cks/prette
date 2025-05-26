#include "prette/settings/settings_event.h"

#include <string>

#include "prette/to_string.h"

namespace prt {
#define DEFINE_TOSTRING(Name)                       \
  auto Name##Event::ToString() const->std::string { \
    return ToStringHelper<Name##Event>{};           \
  }
FOR_EACH_SETTINGS_EVENT(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
}  // namespace prt