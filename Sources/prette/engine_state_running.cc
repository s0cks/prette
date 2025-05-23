#include "prette/engine.h"
#include "prette/engine_event.h"
#include "prette/engine_state.h"
#include "prette/gfx.h"
#include "prette/tick.h"

namespace prt {
#define __ engine->

ENGINE_STATE_ENTER_F(Running) {
  Engine::PublishRunningEvent();
  on_tick_ = __ OnTick().subscribe([this](const Tick& tick) {
    const auto engine = GetEngine();
    OnTick(engine, tick, engine->GetCurrentTick());
  });
  __ StartTicker();
  __ GetLoop().RunDefault();
}

ENGINE_STATE_TICK_F(Running) {
  // pre-tick
  Engine::PublishPreTickEvent();
#ifdef PRT_GLFW
  glfwPollEvents();
#endif  // PRT_GLFW

  // tick-logic
  Engine::PublishTickEvent(current, previous);

  // post-tick
  Engine::PublishPostTickEvent(current);
}

ENGINE_STATE_EXIT_F(Running) {
  on_tick_.unsubscribe();
  __ ticker_.Stop();
}

#undef __
}  // namespace prt