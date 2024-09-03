#include <backward.hpp>

#include "prette/crash_report.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_state.h"

namespace prt::engine {
  ErrorState::ErrorState(Engine* engine):
    EngineState(engine) {
  }

  ErrorState::~ErrorState() {
    
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