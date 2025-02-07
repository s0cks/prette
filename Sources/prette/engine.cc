#include "prette/engine.h"

#include <uv.h>

#include "prette/common.h"
#include "prette/gfx.h"
#include "prette/renderer.h"
#include "prette/runtime.h"
#include "prette/thread_local.h"
#include "prette/to_string.h"

namespace prt::engine {
auto PreInitEvent::ToString() const -> std::string {
  ToStringHelper<PreInitEvent> helper;
  helper.AddFieldPtr("engine", GetEngine());
  return helper;
}

auto PostInitEvent::ToString() const -> std::string {
  ToStringHelper<PostInitEvent> helper;
  helper.AddFieldPtr("engine", GetEngine());
  return helper;
}

auto PreTickEvent::ToString() const -> std::string {
  ToStringHelper<PreTickEvent> helper;
  helper.AddFieldPtr("engine", (const void*)GetEngine());
  return helper;
}

auto TickEvent::ToString() const -> std::string {
  ToStringHelper<TickEvent> helper;
  helper.AddFieldPtr("engine", GetEngine());
  helper.AddFieldRef("current", GetCurrentTick());
  helper.AddFieldRef("previous", GetPreviousTick());
  return helper;
}

auto PostTickEvent::ToString() const -> std::string {
  ToStringHelper<PreTickEvent> helper;
  helper.AddFieldPtr("engine", GetEngine());
  helper.AddFieldRef("tick", GetTick());
  return helper;
}

auto TerminatingEvent::ToString() const -> std::string {
  ToStringHelper<PreTickEvent> helper;
  helper.AddFieldPtr("engine", GetEngine());
  return helper;
}

auto TerminatedEvent::ToString() const -> std::string {
  ToStringHelper<PreTickEvent> helper;
  helper.AddFieldPtr("engine", GetEngine());
  return helper;
}

#define __ engine->

ENGINE_STATE_F(Init) {
  __ PublishPreInitEvent();
  DLOG(INFO) << "init.";
  __ PublishPostInitEvent();
}

RunningState::RunningState(Engine* engine) :
  EngineState(engine),
  ticker_(&(engine->GetLoop())) {
  ticker_.OnTick()
      .map([engine, this](Tick tick) {
        return new TickEvent(engine, tick, ticker_.GetPreviousTick());
      })
      .subscribe([engine](TickEvent* event) {
        __ PublishEvent(event);
      });
}

ENGINE_STATE_F(Running) {
  DLOG(INFO) << "running.";
  engine->GetLoop().RunDefault();
  ticker_.Close();
}

ENGINE_STATE_F(Paused) {
  DLOG(INFO) << "paused.";
}

ENGINE_STATE_F(Terminating) {
  DLOG(INFO) << "terminating.";
}

ENGINE_STATE_F(Error) {
  try {
    std::rethrow_exception(GetCause());
  } catch (const std::exception& exc) {
    LOG(ERROR) << "error: " << exc.what();
  }
}

#undef __

Engine::Engine() :
  EngineEventSource(),
  loop_(),
  running_(false),
  state_(nullptr),
  events_() {}

Engine::~Engine() {
  on_tick_.unsubscribe();
}

auto Engine::CancelTransitionTo(EngineState* new_state) -> bool {
  ASSERT(new_state);
  DLOG(WARNING) << "cancelling transition to: " << new_state->GetStateName();
  return false;
}

auto Engine::TransitionTo(EngineState* new_state) -> bool {
  ASSERT(new_state);
  const auto current_state = state_;
  if (HasState() && GetState()->Equals(new_state))
    return CancelTransitionTo(new_state);
  GetState()->Stop();
  SetState(new_state);
  return true;
}

void Engine::RunState(EngineState* state) {
  ASSERT(state);
  DLOG(INFO) << state->GetStateName() << " started.";
  SetState(state);
  state->Execute(this);
  const auto& duration = state->GetDurationSeries();
  using namespace units::time;
  DLOG(INFO) << state->GetStateName() << " done in " << nanosecond_t(duration.last())
             << ", avg=" << nanosecond_t(duration.average()) << ", max=" << nanosecond_t(duration.max())
             << ", min=" << nanosecond_t(duration.min());  // NOLINT(cppcoreguidelines-narrowing-conversions)
}

void Engine::Run() {
  RunState<InitState>();
  RunState<RunningState>();
  RunState<TerminatingState>();
}

void Engine::Shutdown(const std::exception_ptr& cause) {
  return Terminate();
}

void Engine::PublishEvent(EngineEvent* event) const {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

auto Engine::OnEvent() const -> EngineEventObservable {
  return events_.get_observable();
}

static ThreadLocal<Engine> engine_;

static inline void SetEngine(Engine* engine) {
  ASSERT(engine);
  engine_.Set(engine);
}

auto GetEngine() -> Engine* {
  return engine_.Get();
}

void InitEngine() {
  const auto engine = new Engine();
  ASSERT(engine);
#ifdef PRT_GLFW
  engine->OnTick().subscribe([](TickEvent* event) {
    glfwPollEvents();
    Renderer::DrawFrame(Runtime::GetVkLogicalDevice());
  });
#endif  // PRT_GLFW
  SetEngine(engine);
}
}  // namespace prt::engine