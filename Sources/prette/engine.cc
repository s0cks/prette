#include "prette/engine.h"

#include <cctype>
#include <cstdlib>
#include <lua.h>
#include <memory>
#include <uv.h>

#include "prette/common.h"
#include "prette/crash_report.h"
#include "prette/engine_event.h"
#include "prette/engine_state.h"
#include "prette/rx.h"
#include "prette/thread_local.h"
#include "prette/uv/utils.h"

namespace prt {
static EngineEventSubject events_{};
static ThreadLocal<Engine> engine_;

static inline auto SetEngine(Engine* engine) -> Engine* {
  ASSERT(engine);
  engine_.Set(engine);
  return engine;
}

auto OnEngineEvent() -> EngineEventObservable {
  return events_.get_observable();
}

void Engine::PublishEvent(EngineEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

void Engine::OnShutdown(uv_async_t* handle) {
  ASSERT(handle);
  Unwrap(handle)->Terminate();
}

void Engine::Terminate() {
  if (cause_)
    return SetState<ErrorState>(cause_);
  return SetState<TerminatingState>();
}

static inline auto CreateTickDeltaObservable(const Engine* engine) -> rx::observable<uint64_t> {
  return OnTickEvent().map([](TickEvent* event) {
    return (event->GetTimeSinceLast()).value();
  });
}

Engine::Engine() :
  loop_(),
  ticker_(&loop_),
  scheduler_(&loop_),
#ifdef PRT_DEBUG
  tick_profiler_(CreateTickDeltaObservable(this)),
#endif  // PRT_DEBUG
  on_shutdown_(loop_, &OnShutdown, this) {
}

auto Engine::Run() -> int {
  SetState<InitState>();
  if (HasState()) {
    ExitState();
    ClearState();
  }
  return EXIT_SUCCESS;
}

void Engine::Shutdown(std::shared_ptr<CrashReportCause> cause) {
  cause_ = cause;
  on_shutdown_.Send();
}

auto Engine::Schedule(uv::Work* work) -> uv::Work* {
  return scheduler_.Schedule(work);
}

auto Engine::Schedule(uv::AnyWork::OnWorkCallback on_work) -> uv::Work* {
  return scheduler_.Schedule(on_work);
}

auto Engine::Schedule(uv::AnyWork::OnWorkCallback on_work, uv::AnyWork::OnWorkFinishedCallback on_finished)
    -> uv::Work* {
  return scheduler_.Schedule(on_work, on_finished);
}

void Engine::Init() {
  ASSERT(!IsEngineInitialized());
  SetEngine(new Engine());
  ASSERT(IsEngineInitialized());
}

auto IsEngineInitialized() -> bool {
  return engine_.Get() != nullptr;
}

auto GetEngine() -> Engine* {
  ASSERT(IsEngineInitialized());
  return engine_.Get();
}
}  // namespace prt