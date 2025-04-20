#include "prette/engine.h"

#include <GLFW/glfw3.h>
#include <lua.h>
#include <uv.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <exception>
#include <operators/rx-observe_on.hpp>
#include <rx-observable.hpp>

#include "prette/common.h"
#include "prette/crash_report.h"
#include "prette/engine_event.h"
#include "prette/exception.h"
#include "prette/gfx.h"
#include "prette/lua.h"
#include "prette/prette.h"
#include "prette/renderer.h"
#include "prette/signals.h"
#include "prette/thread_local.h"
#include "prette/to_string.h"
#include "prette/uv/utils.h"
#include "prette/window.h"

namespace prt {
static EngineEventSubject events_{};

auto OnEngineEvent() -> EngineEventObservable {
  return events_.get_observable();
}

void Engine::PublishEvent(EngineEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

static inline auto GetLowercaseStateName(const std::unique_ptr<EngineState>& state) -> std::string {
  std::string name(state->GetStateName());
  std::ranges::transform(name, std::begin(name), [](const char c) {
    return tolower(c);
  });
  return name;
}

void Engine::EnterState(const EngineStatePtr& state) {
  ASSERT(state);
  state->EnterState(this);
}

void Engine::ExitState(const EngineStatePtr& state) {
  ASSERT(state);
  state->ExitState(this);
}

void Engine::OnShutdown(uv_async_t* handle) {
  ASSERT(handle);
  const auto engine = uv::GetHandleData<uv_async_t, Engine>(handle);
  ASSERT(engine);
  engine->Terminate();
}

void Engine::Terminate() {
  if (cause_)
    return SetState<ErrorState>(cause_);
  return SetState<TerminatedState>();
}

static inline auto CreateTickDeltaObservable(const Engine* engine) -> rx::observable<uint64_t> {
  return OnTickEvent().map([](TickEvent* event) {
    return (event->GetTimeSinceLast()).value();
  });
}

Engine::Engine() :
  loop_(),
  ticker_(&loop_),
#ifdef PRT_DEBUG
  tick_profiler_(CreateTickDeltaObservable(this)),
#endif  // PRT_DEBUG
  on_shutdown_(loop_, &OnShutdown, this) {
}

Engine::~Engine() {
  on_tick_.unsubscribe();
}

auto Engine::Run() -> int {
  SetState<InitState>();
  GetLoop().RunDefault();
  if (state_)
    ExitState(state_);
  return EXIT_SUCCESS;
}

void Engine::Shutdown(std::shared_ptr<CrashReportCause> cause) {
  cause_ = cause;
  on_shutdown_.Send();
}

static ThreadLocal<Engine> engine_;

static inline auto SetEngine(Engine* engine) -> Engine* {
  ASSERT(engine);
  engine_.Set(engine);
  return engine;
}

auto Engine::Get() -> Engine* {
  return engine_.Get();
}

void Engine::Init() {
  SetEngine(new Engine());
}
}  // namespace prt