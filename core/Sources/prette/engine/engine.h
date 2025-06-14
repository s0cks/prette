#ifndef PRT_ENGINE_H
#define PRT_ENGINE_H

// clang-format off
#include "prette/assertions.h"
#include "prette/rx.h"  // IWYU pragma: keep
// clang-format on

#include <exception>
#include <marl/defer.h>
#include <marl/event.h>
#include <marl/scheduler.h>
#include <marl/waitgroup.h>
#include <memory>
#include <units.h>

#include "prette/callback.h"
#include "prette/common.h"
#include "prette/crash_report.h"
#include "prette/engine/engine_event.h"
#include "prette/engine/engine_states.h"
#include "prette/next_tick.h"
#include "prette/scheduler.h"
#include "prette/system.h"
#include "prette/tick.h"
#include "prette/tick_profiler.h"
#include "prette/ticker.h"
#include "prette/uv/async.h"
#include "prette/uv/handle.h"
#include "prette/uv/utils.h"

namespace prt {
class Engine;

auto GetEngineEventObservable() -> EngineEventObservable;

template <typename... ArgN>
static inline auto OnEngineEvent(ArgN... args) -> rx::composite_subscription {
  return GetEngineEventObservable().subscribe(args...);
}

#define DEFINE_ON_EVENT(Name)                                                             \
  template <typename... ArgN>                                                             \
  static inline auto On##Name##Event()->Name##EventObservable {                           \
    return GetEngineEventObservable().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }                                                                                       \
  template <typename... ArgN>                                                             \
  static inline auto On##Name(ArgN... args)->rx::composite_subscription {                 \
    return On##Name##Event().subscribe(args...);                                          \
  }
FOR_EACH_ENGINE_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

static inline auto GetNonTickEngineEventObservable() -> EngineEventObservable {
  return GetEngineEventObservable().filter([](EngineEvent* event) {
    return event && !(event->IsTickEvent() || event->IsPreTickEvent() || event->IsPostTickEvent());
  });
}

template <typename... ArgN>
static inline auto OnNonTickEngineEvent(ArgN... args) -> rx::composite_subscription {
  return GetNonTickEngineEventObservable().subscribe(args...);
}

template <class State>
class StateMachineTemplate {
 public:
  using StatePtr = std::unique_ptr<State>;

 private:
  StatePtr state_;

 protected:
  StateMachineTemplate() = default;

  virtual void EnterState() = 0;
  virtual void ExitState() = 0;

  template <class S, typename... Args>
  inline void SetState(Args... args) {
    if (state_)
      ExitState();
    state_ = S::New(args...);
    ASSERT(state_);
    EnterState();
  }

  inline void ClearState() {
    state_ = nullptr;
  }

 public:
  virtual ~StateMachineTemplate() = default;

  inline auto HasState() const -> bool {
    return state_ != nullptr;
  }

  auto GetState() const -> const StatePtr& {
    return state_;
  }
};

using InitCallback = Callback<void>;
using ShutdownCallback = Callback<void>;

static constexpr const auto kTargetTicksPerSecond = 120;
static constexpr const auto kTargetTickRate = NSEC_PER_SEC / kTargetTicksPerSecond;
class Engine : public StateMachineTemplate<EngineState> {
  friend class EngineTicker;
#define DEFINE_FRIEND(Name) friend class Name##State;
  FOR_EACH_ENGINE_STATE(DEFINE_FRIEND)
#undef DEFINE_FRIEND
  DEFINE_NON_COPYABLE_TYPE(Engine);

 private:
  static void PublishEvent(EngineEvent* event);

  template <class E, typename... Args>
  static inline void Publish(Args... args) {
    E event(args...);
    return PublishEvent(&event);
  }

#define DEFINE_PUBLISH(Name)                              \
  template <typename... Args>                             \
  static inline void Publish##Name##Event(Args... args) { \
    return Publish<Name##Event>(args...);                 \
  }
  FOR_EACH_ENGINE_EVENT(DEFINE_PUBLISH);
#undef DEFINE_PUBLISH

  static inline void OnShutdown(uv_async_t* handle) {
    ASSERT(handle);
    Unwrap(handle)->Terminate();
  }

 private:
  marl::Scheduler mscheduler_;
  uv::Loop loop_;
  RateLimitedTicker<kTargetTickRate> ticker_;
  std::shared_ptr<CrashReportCause> cause_ = nullptr;
  uv::Async on_shutdown_;
  InitCallback* init_callbacks_ = nullptr;
  ShutdownCallback* shutdown_callbacks_ = nullptr;
  System* systems_ = nullptr;
  Scheduler scheduler_;
  NextTickQueue next_tick_queue_{};
#ifdef PRT_DEBUG
  TickProfiler tick_profiler_;
#endif  // PRT_DEBUG
#ifdef PRT_ENABLE_LUA
  rx::subscription on_lua_init_{};
#endif  // PRT_ENABLE_LUA

  void EnterState() override {
    return GetState()->EnterState(this);
  }

  void ExitState() override {
    return GetState()->ExitState(this);
  }

#define DEFINE_SET_STATE(Name)                 \
  template <typename... Args>                  \
  inline void Set##Name##State(Args... args) { \
    return SetState<Name##State>(args...);     \
  }
  FOR_EACH_ENGINE_STATE(DEFINE_SET_STATE)
#undef DEFINE_SET_STATE

  auto GetInitCallbacks() -> InitCallback* {
    return init_callbacks_;
  }

  auto GetShutdownCallbacks() -> ShutdownCallback* {
    return shutdown_callbacks_;
  }

  inline void StartTicker() {
    return ticker_.Start();
  }

  void Terminate();

  inline void StopLoop() {
    return loop_.Stop();
  }

  inline void StopTicker() {
    return ticker_.Stop();
  }

  inline void Stop() {
    StopTicker();
    StopLoop();
  }

  auto GetNextTickQueue() const -> const NextTickQueue& {
    return next_tick_queue_;
  }

  void ClearNextTickQueue() {
    next_tick_queue_.clear();
  }

 public:
  Engine();
  ~Engine() override;

  auto GetScheduler() -> Scheduler& {
    return scheduler_;
  }

  auto GetMarlScheduler() -> marl::Scheduler& {
    return mscheduler_;
  }

  auto GetLoop() const -> const uv::Loop& {
    return loop_;
  }

  auto GetLoop() -> uv::Loop& {
    return loop_;
  }

  auto Run() -> int;
  void Shutdown(std::shared_ptr<CrashReportCause> cause);
  auto Schedule(uv::Work* work) -> uv::Work*;
  auto Schedule(uv::AnyWork::OnWorkCallback on_work) -> uv::Work*;
  auto Schedule(uv::AnyWork::OnWorkCallback on_work, uv::AnyWork::OnWorkFinishedCallback on_finished) -> uv::Work*;

  inline void Shutdown(const std::exception_ptr& cause = nullptr, const int depth = CrashReportCause::kDefaultDepth,
                       const int offset = CrashReportCause::kDefaultOffset) {
    if (cause)
      return Shutdown(CrashReportCause::New(cause, depth, offset));
    return Shutdown((std::shared_ptr<CrashReportCause>)nullptr);
  }

  auto GetEpochTick() const -> const Tick& {
    return ticker_.GetEpochTick();
  }

  auto GetCurrentTick() const -> const Tick& {
    return ticker_.GetCurrentTick();
  }

  auto GetPreviousTick() const -> const Tick& {
    return ticker_.GetPreviousTick();
  }

  auto GetTicksPerSecond() const -> const TicksPerSecond& {
    return ticker_.GetTicksPerSecond();
  }

  auto OnTick() const -> rx::observable<Tick> {
    return ticker_.OnTick();
  }

  auto OnTickProfilerStats() const -> rx::observable<TickStats> {
    return tick_profiler_.OnStats();
  }

  void AddInitCallback(InitCallback* rhs) {
    ASSERT(rhs);
    return InitCallback::Append(&init_callbacks_, rhs);
  }

  inline void AddInitCallback(InitCallback::FunctionType rhs) {
    return AddInitCallback(new InitCallback(rhs));
  }

  void RemoveInitCallback(InitCallback* rhs) {
    ASSERT(rhs);
    return InitCallback::Remove(&init_callbacks_, rhs);
  }

  void AddShutdownCallback(ShutdownCallback* rhs) {
    ASSERT(rhs);
    return ShutdownCallback::Append(&shutdown_callbacks_, rhs);
  }

  inline void AddShutdownCallback(ShutdownCallback::FunctionType rhs) {
    return AddShutdownCallback(new ShutdownCallback(rhs));
  }

  void RemoveShutdownCallback(ShutdownCallback* rhs) {
    ASSERT(rhs);
    return ShutdownCallback::Remove(&shutdown_callbacks_, rhs);
  }

  auto GetSystemsList() const -> System* {
    return systems_;
  }

  void AddSystem(System* rhs) {
    ASSERT(rhs);
    Append(&systems_, rhs);
  }

  void RemoveSystem(System* rhs) {
    ASSERT(rhs);
    Remove(&systems_, rhs);
  }

  void OnNextTick(NextTickCallback func) {
    ASSERT(func);
    next_tick_queue_.push_back(func);
  }

 private:
  static inline auto Unwrap(uv_async_t* rhs) -> Engine* {
    ASSERT(rhs);
    return uv::GetHandleData<uv_async_t, Engine>(rhs);
  }

 public:
  static void Init(int argc, char** argv);
};

auto GetEngine() -> Engine*;
auto IsEngineInitialized() -> bool;

static inline void OnNextTick(NextTickCallback callback) {
  return GetEngine()->OnNextTick(callback);
}
}  // namespace prt

#endif  // PRT_ENGINE_H