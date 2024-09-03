#include "prette/engine/engine.h"

#include "prette/common.h"
#include "prette/thread_local.h"
#include "prette/render/renderer.h"

#include "prette/keyboard/keyboard.h"

namespace prt::engine {
  Engine::Engine():
    EngineEventPublisher(),
    loop_(),
    running_(false),
    state_(nullptr),
    ticker_(&loop_),
    cause_() {
  }

  Engine::~Engine() {
    on_tick_.unsubscribe();
  }

  void Engine::RunState(EngineState* state) {
    PRT_ASSERT(state);
    DLOG(INFO) << state->GetName() << " started.";
    SetState(state);
    state->Run();
    const auto& duration = state->GetDurationSeries();
    using namespace units::time;
    DLOG(INFO) << state->GetName() << " done in " << nanosecond_t(duration.last()) << ", avg=" << nanosecond_t(duration.average()) << ", max=" << nanosecond_t(duration.max()) << ", min=" << nanosecond_t(duration.min());
  }

  void Engine::Run() {
    RunState<InitState>();
    RunState<RunningState>();
    if(HasCause())
      RunState<ErrorState>();
    RunState<TerminatedState>();
  }

  void Engine::Shutdown(const std::exception_ptr& cause) {
    if(cause)
      SetCause(cause);
    if(HasState())
      GetState()->Stop();
  }

  static ThreadLocal<Engine> engine_;

  static inline void
  SetEngine(Engine* engine) {
    PRT_ASSERT(engine);
    engine_.Set(engine);
  }

  Engine* GetEngine() {
    return engine_.Get();
  }

  void InitEngine() {
    SetEngine(new Engine());
  }
}