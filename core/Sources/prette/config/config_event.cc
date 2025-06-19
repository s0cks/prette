#include "prette/config/config_event.h"

#include <string>

#include "prette/config/config_state.h"
#include "prette/to_string.h"

namespace prt {
#define DEFINE_TOSTRING(Name)                       \
  auto Name##Event::ToString() const->std::string { \
    ToStringHelper<Name##Event> helper{};           \
    return helper;                                  \
  }
FOR_EACH_CONFIG_STATE(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
}  // namespace prt