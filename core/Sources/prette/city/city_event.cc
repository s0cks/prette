#include "prette/city/city_event.h"

#include "prette/city/city.h"  // IWYU pragma: keep
#include "prette/to_string.h"

namespace prt {
#define DEFINE_TOSTRING(Name)                       \
  auto Name##Event::ToString() const->std::string { \
    ToStringHelper<Name##Event> helper{};           \
    helper.AddFieldPtr("city", GetCity());          \
    return helper;                                  \
  }
FOR_EACH_CITY_EVENT(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
}  // namespace prt