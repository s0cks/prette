#include "prette/script_engine.h"

#include <chrono>

#include "prette/common.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"

namespace prt {
ScriptEngine::ScriptEngine() {
  on_pre_init_ = OnPreInit([this](PreInitEvent* event) {
    VLOG(1) << "initializing " << GetLanguage() << " script engine v" << GetLanguageVersion() << "...";
    PRT_PROFILING_BEGIN(init);
    Init();
    PRT_PROFILING_END(init);
    DVLOG(1) << GetLanguage() << " v" << GetLanguageVersion() << " initialized in "
             << std::chrono::duration_cast<std::chrono::milliseconds>(init_duration).count() << "ms.";
  });
  on_terminating_ = OnTerminating([this](TerminatingEvent* event) {
    Finalize();
  });
}

ScriptEngine::~ScriptEngine() {
  on_pre_init_.unsubscribe();
  on_terminating_.unsubscribe();
}

void ScriptEngine::Init() {
  // do something?
}

void ScriptEngine::Finalize() {
  // do something?
}
}  // namespace prt