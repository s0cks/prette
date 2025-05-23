#include "prette/engine.h"
#include "prette/engine_state.h"

namespace prt {
#define __ engine->

ENGINE_STATE_ENTER_F(Paused) {
  __ PublishPausedEvent();
}

#undef __
}  // namespace prt