#include "prette/engine/engine_state_terminating.h"

#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/engine/engine_state.h"

namespace prt {
#define __ engine->

ENGINE_STATE_ENTER_F(Terminating) {
  Engine::PublishTerminatingEvent();
  __ Stop();
  __ SetTerminatedState();
}

#undef __
}  // namespace prt