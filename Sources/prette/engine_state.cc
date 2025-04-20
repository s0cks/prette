#include "prette/engine_state.h"

#include "prette/engine.h"
#include "prette/gfx.h"
#include "prette/lua.h"
#include "prette/renderer.h"
#include "prette/world.h"

namespace prt {
#define __ engine->

ENGINE_STATE_ENTER_F(Init) {
  Engine::Publish<PreInitEvent>();

  LuaState::Get()->ExecuteScript("boot.lua");
  if (ShouldEnableLuaEventLogging())
    LuaState::Get()->ExecuteScript("event-logging.lua");

  __ SetState<RunningState>();
}

ENGINE_STATE_TICK_F(Init) {}

ENGINE_STATE_EXIT_F(Init) {
  Engine::Publish<PostInitEvent>();
}

ENGINE_STATE_ENTER_F(Running) {
  __ ticker_.Start();
}

#ifdef PRT_DEBUG

#define BEGIN_GRAPHICS_SECTION if (FLAGS_gfx) {
#define END_GRAPHICS_SECTION   }

#else

#define BEGIN_GRAPHICS_SECTION
#define END_GRAPHICS_SECTION

#endif  // PRT_DEBUG

ENGINE_STATE_TICK_F(Running) {
  // pre-tick
  Engine::Publish<PreTickEvent>();
  glfwPollEvents();

  // tick-logic
  Engine::Publish<TickEvent>(current, previous);
  BEGIN_GRAPHICS_SECTION
  Renderer::DrawFrame(Driver::Get(), current, previous);
  END_GRAPHICS_SECTION

  // post-tick
  Engine::Publish<PostTickEvent>(current);
}

ENGINE_STATE_EXIT_F(Running) {
  // do nothig
}

ENGINE_STATE_ENTER_F(Paused) {
  __ Stop();
}

ENGINE_STATE_TICK_F(Paused) {
  // do nothing
}

ENGINE_STATE_EXIT_F(Paused) {
  // do nothing
}

ENGINE_STATE_ENTER_F(Terminated) {
  Engine::Publish<TerminatingEvent>();
  __ Stop();
}

ENGINE_STATE_TICK_F(Terminated) {
  // do nothing
}

ENGINE_STATE_EXIT_F(Terminated) {
  Engine::Publish<TerminatedEvent>();
}

ENGINE_STATE_ENTER_F(Error) {
  __ SetState<TerminatedState>();
}

ENGINE_STATE_TICK_F(Error) {
  // do nothing
}

ENGINE_STATE_EXIT_F(Error) {
  LOG(ERROR) << "an exception has occurred.";
  CrashReport report(GetCause());
  report.Print();
  Engine::Publish<ErrorEvent>();
}

#undef __
}  // namespace prt