#include "prette/engine/engine_state_error.h"

#include "prette/common.h"
#include "prette/crash_report.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/engine/engine_state.h"

namespace prt {
#define __ engine->

ENGINE_STATE_ENTER_F(Error) {
  Engine::PublishErrorEvent();
  __ SetTerminatedState();
}

ENGINE_STATE_EXIT_F(Error) {
  LOG(ERROR) << "an exception has occurred.";
  CrashReport report(GetCause());
  report.Print();
}

#undef __
}  // namespace prt