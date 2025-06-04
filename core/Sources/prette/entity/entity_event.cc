#include "prette/entity/entity_event.h"

#include "prette/to_string.h"

namespace prt {
#define DEFINE_TO_STRING(Name)                      \
  auto Name##Event::ToString() const->std::string { \
    ToStringHelper<Name##Event> helper{};           \
    return helper;                                  \
  }
FOR_EACH_ENTITY_EVENT(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING

}  // namespace prt