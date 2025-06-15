#include "prette/engine/engine_state_init.h"

#include "prette/city_manager.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/engine/engine_state.h"
#include "prette/lua.h"

namespace prt {
#define __ engine->

ENGINE_STATE_ENTER_F(Init) {
  Engine::PublishPreInitEvent();

  InitCallback::ApplyAll(engine->GetInitCallbacks());
#ifdef PRT_ENABLE_LUA
  GetLua()->ExecuteScript("boot.lua");
  if (ShouldEnableLuaEventLogging())
    GetLua()->ExecuteScript("event-logging.lua");
#endif  // PRT_ENABLE_LUA

  CityManager::Init();

  __ SetRunningState();
}

ENGINE_STATE_EXIT_F(Init) {
  Engine::PublishPostInitEvent();
}
#undef __
}  // namespace prt