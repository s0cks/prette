#include "prette/engine/engine_state_paused.h"

#include "prette/engine/engine.h"
#include "prette/engine/engine_state.h"

namespace prt {
#define __ engine->

ENGINE_STATE_ENTER_F(Paused) {
  __ PublishPausedEvent();
}

#undef __
}  // namespace prt