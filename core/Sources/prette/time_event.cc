#include "prette/time_event.h"

#include "prette/to_string.h"

namespace prt {
#define DEFINE_TOSTRING(Name)                       \
  auto Name##Event::ToString() const->std::string { \
    return ToStringHelper<Name##Event>{};           \
  }
FOR_EACH_TIME_EVENT(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
}  // namespace prt