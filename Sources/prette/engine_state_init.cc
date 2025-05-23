#include "prette/engine.h"
#include "prette/engine_event.h"
#include "prette/engine_state.h"
#include "prette/lua.h"

namespace prt {
#define __ engine->

ENGINE_STATE_ENTER_F(Init) {
  Engine::PublishPreInitEvent();
  InitCallback::ApplyAll(engine->GetInitCallbacks());

#ifdef PRETTE_ENABLE_LUA
  LuaState::Get()->ExecuteScript("boot.lua");
  if (ShouldEnableLuaEventLogging())
    LuaState::Get()->ExecuteScript("event-logging.lua");
#endif  // PRETTE_ENABLE_LUA

  __ SetRunningState();
}

ENGINE_STATE_EXIT_F(Init) {
  Engine::PublishPostInitEvent();
}
#undef __
}  // namespace prt