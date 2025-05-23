#include "prette/lua_event.h"

#ifdef PRETTE_ENABLE_LUA

#include <string>

#include "prette/to_string.h"

namespace prt {
auto LuaStateInitEvent::ToString() const -> std::string {
  return ToStringHelper<LuaStateInitEvent>{};
}

auto LuaStateDestroyedEvent::ToString() const -> std::string {
  return ToStringHelper<LuaStateDestroyedEvent>{};
}
}  // namespace prt

#endif  // PRETTE_ENABLE_LUA