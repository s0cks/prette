#include "prette/engine_event.h"

#include <string>

#include "prette/engine_state.h"
#include "prette/to_string.h"

namespace prt {
#define DEFINE_TOSTRING(Name)                       \
  auto Name##Event::ToString() const->std::string { \
    return ToStringHelper<Name##Event>{};           \
  }

DEFINE_TOSTRING(PreInit);
DEFINE_TOSTRING(PostInit);
DEFINE_TOSTRING(PreTick);
FOR_EACH_ENGINE_STATE(DEFINE_TOSTRING);
#undef DEFINE_TOSTRING

auto TickEvent::ToString() const -> std::string {
  ToStringHelper<TickEvent> helper;
  helper.AddFieldRef("current", GetCurrentTick());
  helper.AddFieldRef("previous", GetPreviousTick());
  helper.AddFieldRef("delta", GetTimeSinceLast());
  return helper;
}

auto PostTickEvent::ToString() const -> std::string {
  ToStringHelper<PostTickEvent> helper;
  helper.AddFieldRef("tick", GetTick());
  return helper;
}
}  // namespace prt