#include "prette/event.h"

#include <lauxlib.h>
#include <lua.h>

#include "prette/lua.h"

namespace prt {
static inline auto CheckEvent(lua_State* L, const int index) -> Event const* {
  ASSERT(L);
  lua_getmetatable(L, 1);
  lua_getfield(L, -1, "__data");
  luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
  return (Event const*)lua_topointer(L, -1);
}

LUA_F(event_toString) {
  const auto event = CheckEvent(L, 1);
  ASSERT(event);
  const auto value = event->ToString();
  lua_pushstring(L, value.c_str());
  return 1;
}

void Event::ToTable(lua_State* L) const {
  ASSERT(L);
  lua_createtable(L, 0, 1);

  luaL_newmetatable(L, GetName());
  lua_pushlightuserdata(L, (void*)this);
  lua_setfield(L, -2, "__data");

  lua_pushcfunction(L, &lua_event_toString);
  lua_setfield(L, -2, "__tostring");
  lua_setmetatable(L, -2);
}
}  // namespace prt