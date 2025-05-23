#include "prette/world_event.h"

#include <string>

#include "prette/to_string.h"
#include "prette/world_state.h"

namespace prt {
#define DEFINE_TOSTRING(Name)                       \
  auto Name##Event::ToString() const->std::string { \
    return ToStringHelper<Name##Event>{};           \
  }
FOR_EACH_WORLD_STATE(DEFINE_TOSTRING);
#undef DEFINE_TOSTRING
}  // namespace prt