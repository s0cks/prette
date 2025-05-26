#include "prette/engine/engine_lua_bindings.h"

#include "prette/assertions.h"

#ifdef PRT_ENABLE_LUA

#include <exception>

#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/exception.h"
#include "prette/lua.h"

namespace prt {
#define LUA_ENGINE_F(Name) LUA_F(engine_##Name)

LUA_ENGINE_F(getStateName) {
  const auto engine = GetEngine();
  ASSERT(engine);
  lua_pushstring(L, engine->GetState()->GetStateName());
  return 1;
}

LUA_ENGINE_F(shutdown) {
  const auto engine = GetEngine();
  ASSERT(engine);
  std::exception_ptr cause = nullptr;
  if (lua_gettop(L) > 0) {
    luaL_checktype(L, 1, LUA_TSTRING);
    cause = Exception::New(lua_tostring(L, 1));
  }
  engine->Shutdown(cause);
  return 0;
}

LUA_ENGINE_F(onEvent) {
  const auto engine = GetEngine();
  ASSERT(engine);
  OnEngineEvent(CreateSubscriber<EngineEvent>(L));
  return 0;
}

#define DEFINE_ON_EVENT_FUNC(Name)              \
  LUA_ENGINE_F(on##Name##Event) {               \
    On##Name(CreateSubscriber<Name##Event>(L)); \
    return 0;                                   \
  }
FOR_EACH_ENGINE_EVENT(DEFINE_ON_EVENT_FUNC);
#undef DEFINE_ON_EVENT_FUNC
#undef LUA_ENGINE_F

// clang-format off
// NOLINTNEXTLINE
DEFINE_LUALIB(Engine) {
#define LUA_ENGINE_F(Name) \
  {.name = #Name, .func = &lua_engine_##Name }

  LUA_ENGINE_F(onEvent),
#define DEFINE_ON_EVENT(Name) \
  LUA_ENGINE_F(on##Name##Event),
  FOR_EACH_ENGINE_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
#undef LUA_ENGINE_F
};

// clang-format on

void EngineModule::Init(lua_State* L) {
  InitModule(L, "engine", "Engine", kEngineLib);
}
}  // namespace prt

#endif  // PRT_ENABLE_LUA