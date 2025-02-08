#include "prette/engine.h"

#include <GLFW/glfw3.h>
#include <uv.h>

#include <algorithm>
#include <cstdlib>
#include <exception>
#include <operators/rx-observe_on.hpp>
#include <rx-observable.hpp>

#include "prette/command_pool.h"
#include "prette/common.h"
#include "prette/crash_report.h"
#include "prette/gfx.h"
#include "prette/pipeline.h"
#include "prette/renderer.h"
#include "prette/signals.h"
#include "prette/swap_chain.h"
#include "prette/thread_local.h"
#include "prette/to_string.h"
#include "prette/uv/utils.h"
#include "prette/window.h"

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
  helper.AddFieldRef("delta", GetTimeSinceLast());
  return helper;
}

auto PostTickEvent::ToString() const -> std::string {
  ToStringHelper<PostTickEvent> helper;
  helper.AddFieldPtr("engine", GetEngine());
  helper.AddFieldRef("tick", GetTick());
  return helper;
}

auto TerminatingEvent::ToString() const -> std::string {
  ToStringHelper<TerminatingEvent> helper;
  helper.AddFieldPtr("engine", GetEngine());
  return helper;
}

auto TerminatedEvent::ToString() const -> std::string {
  ToStringHelper<TerminatedEvent> helper;
  helper.AddFieldPtr("engine", GetEngine());
  return helper;
}

auto ErrorEvent::ToString() const -> std::string {
  ToStringHelper<ErrorEvent> helper;
  helper.AddFieldPtr("engine", GetEngine());
  return helper;
}

static inline void OnUnhandledException() {
  CrashReport report(CrashReportCause::New(std::current_exception()));
  report.Print();
  LOG(FATAL) << "unhandled exception occured.";
}

#define __ engine->

ENGINE_STATE_ENTER_F(Init) {
  __ PublishPreInitEvent();

  srand(time(nullptr));
  InitSignalHandlers();
  std::set_terminate(OnUnhandledException);
  LOG_IF(FATAL, !SetCurrentThreadName("main")) << "failed to set main thread name.";
  gfx::Init();

  InitWindows();
  const auto driver = InitDriver();
  ASSERT(driver);
  SwapChain::Init(driver);
  Pipeline::Init(driver);
  CommandPool::Init(driver);
  Pipeline::InitBuffers();
  Renderer::Init(driver);

  __ SetState<RunningState>();
}

ENGINE_STATE_TICK_F(Init) {}

ENGINE_STATE_EXIT_F(Init) {
  __ PublishPostInitEvent();
}

ENGINE_STATE_ENTER_F(Running) {
  __ ticker_.Start();
}

ENGINE_STATE_TICK_F(Running) {
  // pre-tick
  __ Publish<PreTickEvent>(engine);
  glfwPollEvents();

  // tick-logic
  __ Publish<TickEvent>(engine, current, previous);

  // post-tick
  const auto driver = GetDriver();
  ASSERT(driver);
  Renderer::DrawFrame(driver);
  __ Publish<PostTickEvent>(engine, current);
}

ENGINE_STATE_EXIT_F(Running) {
  // do nothig
}

ENGINE_STATE_ENTER_F(Paused) {
  DLOG(INFO) << "paused.";
  __ Stop();
}

ENGINE_STATE_TICK_F(Paused) {
  // do nothing
}

ENGINE_STATE_EXIT_F(Paused) {
  // do nothing
}

ENGINE_STATE_ENTER_F(Terminated) {
  __ Stop();
  __ PublishTerminatingEvent();
}

ENGINE_STATE_TICK_F(Terminated) {
  // do nothing
}

ENGINE_STATE_EXIT_F(Terminated) {
  glfwTerminate();
  __ PublishTerminatedEvent();
}

ENGINE_STATE_ENTER_F(Error) {
  __ SetState<TerminatedState>();
}

ENGINE_STATE_TICK_F(Error) {
  // do nothing
}

ENGINE_STATE_EXIT_F(Error) {
  LOG(ERROR) << "an exception has occurred.";
  CrashReport report(GetCause());
  report.Print();
  __ PublishErrorEvent();
}

#undef __

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
  return engine->OnTickEvent().map([](engine::TickEvent* event) {
    return (event->GetTimeSinceLast()).value();
  });
}

Engine::Engine() :
  EngineEventSource(),
  loop_(),
  on_shutdown_(loop_, &OnShutdown, this),
  ticker_(&loop_),
#ifdef PRT_DEBUG
  tick_profiler_(CreateTickDeltaObservable(this)),
#endif  // PRT_DEBUG
  running_(false),
  state_(nullptr),
  events_() {
#ifdef PRT_DEBUG
  OnEvent().subscribe(LogEvent<EngineEvent>(google::INFO, __FILE__, __LINE__));
  OnTickProfilerStats().subscribe(([this](const TickStats stats) {
    DLOG(INFO) << "tps: " << GetTicksPerSecond().per_sec() << "; rate=" << stats;
  }));
#endif  // PRT_DEBUG
}

Engine::~Engine() {
  on_tick_.unsubscribe();
}

auto Engine::Run() -> int {
  SetState<InitState>();
  GetLoop().RunDefault();
  if (state_)
    state_->ExitState(this);
  return EXIT_SUCCESS;
}

void Engine::Shutdown(std::shared_ptr<CrashReportCause> cause) {
  cause_ = cause;
  on_shutdown_.Send();
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
}  // namespace prt::engine