#include "prette/person_event.h"

#include "prette/person.h"  // IWYU pragma: keep
#include "prette/to_string.h"

namespace prt {
#define DEFINE_TOSTRING(Name)                       \
  auto Name##Event::ToString() const->std::string { \
    ToStringHelper<Name##Event> helper{};           \
    helper.AddFieldRef("person", GetPerson());      \
    return helper;                                  \
  }
FOR_EACH_PERSON_EVENT(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
}  // namespace prt