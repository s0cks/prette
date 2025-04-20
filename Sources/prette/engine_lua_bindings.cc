#include "prette/engine.h"
#include "prette/exception.h"

namespace prt {
class EngineModule : LuaModule {
  friend class Engine;
  friend class LuaState;

 private:
  static void Init(lua_State* L);
};

#define LUA_ENGINE_F(Name) LUA_F(engine_##Name)

LUA_ENGINE_F(getStateName) {
  const auto engine = Engine::Get();
  ASSERT(engine);
  lua_pushstring(L, engine->GetState()->GetStateName());
  return 1;
}

LUA_ENGINE_F(shutdown) {
  const auto engine = Engine::Get();
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
  const auto engine = Engine::Get();
  ASSERT(engine);
  OnEngineEvent().subscribe(CreateSubscriber<EngineEvent>(L));
  return 0;
}

#define DEFINE_ON_EVENT_FUNC(Name)                                 \
  LUA_ENGINE_F(on##Name##Event) {                                  \
    On##Name##Event().subscribe(CreateSubscriber<Name##Event>(L)); \
    return 0;                                                      \
  }
FOR_EACH_ENGINE_EVENT(DEFINE_ON_EVENT_FUNC);
#undef DEFINE_ON_EVENT_FUNC
#undef LUA_ENGINE_F

// clang-format off
// NOLINTNEXTLINE
static const struct luaL_Reg kEngineLib[] = {
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

void Engine::InitLua(lua_State* L) {
  EngineModule::Init(L);
}
}  // namespace prt