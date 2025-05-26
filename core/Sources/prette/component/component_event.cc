#include "prette/component/component_event.h"

#include "prette/to_string.h"

namespace prt {
#define DEFINE_TOSTRING(Name)                       \
  auto Name##Event::ToString() const->std::string { \
    return ToStringHelper<Name##Event>{};           \
  }
DEFINE_TOSTRING(RegisterComponents);
#undef DEFINE_TOSTRING

auto ComponentAddedEvent::ToString() const -> std::string {
  ToStringHelper<ComponentAddedEvent> helper{};
  helper.AddFieldRef("entity_id", entity_.GetId());
  return helper;
}

auto ComponentRemovedEvent::ToString() const -> std::string {
  ToStringHelper<ComponentRemovedEvent> helper{};
  helper.AddFieldRef("entity_id", entity_.GetId());
  return helper;
}
}  // namespace prt