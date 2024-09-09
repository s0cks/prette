#include "prette/engine/engine.h"

#include "prette/common.h"
#include "prette/thread_local.h"
#include "prette/engine/engine_state_init.h"
#include "prette/engine/engine_state_error.h"
#include "prette/engine/engine_state_running.h"
#include "prette/engine/engine_state_terminated.h"

namespace prt::engine {
  Engine::Engine():
    EngineEventSource(),
    loop_(),
    running_(false),
    state_(nullptr),
    ticker_(&loop_),
    cause_(),
    events_() {
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
    DLOG(INFO) << state->GetName() << " done in " << nanosecond_t(duration.last()) << ", avg=" << nanosecond_t(duration.average()) << ", max=" << nanosecond_t(duration.max()) << ", min=" << nanosecond_t(duration.min()); // NOLINT(cppcoreguidelines-narrowing-conversions)
  }

  void Engine::Run() {
    RunState<InitState>();
    RunState<RunningState>();
    if(HasCause())
      RunState<ErrorState>();
    RunState<TerminatingState>();
    RunState<TerminatedState>();
  }

  void Engine::Shutdown(const std::exception_ptr& cause) {
    if(cause)
      SetCause(cause);
    if(HasState())
      GetState()->Stop();
  }

  void Engine::PublishEvent(EngineEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    return subscriber.on_next(event);
  }

  auto Engine::OnEvent() const -> EngineEventObservable {
    return events_.get_observable();
  }

  static ThreadLocal<Engine> engine_; //NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

  static inline void
  SetEngine(Engine* engine) {
    PRT_ASSERT(engine);
    engine_.Set(engine);
  }

  auto GetEngine() -> Engine* {
    return engine_.Get();
  }

  void InitEngine() {
    SetEngine(new Engine());
  }
}