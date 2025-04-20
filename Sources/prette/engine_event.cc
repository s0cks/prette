#include "prette/engine_event.h"

#include "prette/to_string.h"

namespace prt {
auto PreInitEvent::ToString() const -> std::string {
  return ToStringHelper<PreInitEvent>{};
}

auto PostInitEvent::ToString() const -> std::string {
  return ToStringHelper<PostInitEvent>{};
}

auto PreTickEvent::ToString() const -> std::string {
  return ToStringHelper<PreTickEvent>{};
}

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

auto TerminatingEvent::ToString() const -> std::string {
  return ToStringHelper<TerminatingEvent>{};
}

auto TerminatedEvent::ToString() const -> std::string {
  return ToStringHelper<TerminatedEvent>{};
}

auto ErrorEvent::ToString() const -> std::string {
  return ToStringHelper<ErrorEvent>{};
}
}  // namespace prt