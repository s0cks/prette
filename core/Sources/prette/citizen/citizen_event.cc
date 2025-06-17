#include "prette/citizen/citizen_event.h"

#include "prette/citizen/citizen.h"  // IWYU pragma: keep
#include "prette/to_string.h"

namespace prt {
#define DEFINE_TOSTRING(Name)                       \
  auto Name##Event::ToString() const->std::string { \
    ToStringHelper<Name##Event> helper{};           \
    helper.AddFieldRef("citizen", GetCitizen());    \
    return helper;                                  \
  }
FOR_EACH_PERSON_EVENT(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
}  // namespace prt