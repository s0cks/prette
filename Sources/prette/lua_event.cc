#include "prette/lua_event.h"

#include "prette/to_string.h"

namespace prt {
auto LuaStateInitEvent::ToString() const -> std::string {
  return ToStringHelper<LuaStateInitEvent>{};
}

auto LuaStateDestroyedEvent::ToString() const -> std::string {
  return ToStringHelper<LuaStateDestroyedEvent>{};
}
}  // namespace prt