#include "prette/engine.h"
#include "prette/engine_event.h"
#include "prette/engine_state.h"

namespace prt {
#define __ engine->

ENGINE_STATE_ENTER_F(Terminating) {
  Engine::PublishTerminatingEvent();
  __ Stop();
  __ SetTerminatedState();
}

#undef __
}  // namespace prt