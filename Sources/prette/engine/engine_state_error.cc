#include "prette/engine/engine_state_error.h"
#include "prette/crash_report.h"
#include "prette/engine/engine.h"

namespace prt::engine {
  ErrorState::ErrorState(Engine* engine):
    EngineState(engine) {
  }

  void ErrorState::Run() {
    const auto engine = GetEngine();
    PRT_ASSERT(engine);
    CrashReport report(engine->GetCause());
    report.Print();
  }

  void ErrorState::Stop() {
    // do stuffs?
  }
}