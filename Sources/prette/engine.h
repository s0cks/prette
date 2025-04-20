#ifndef PRT_ENGINE_H
#define PRT_ENGINE_H

#include <units.h>
#include <uv.h>

#include <memory>

#include "prette/common.h"
#include "prette/crash_report.h"
#include "prette/engine_event.h"
#include "prette/engine_state.h"
#include "prette/lua.h"
#include "prette/tick_profiler.h"
#include "prette/ticker.h"
#include "prette/uv/utils.h"

namespace prt {
class LuaState;

class Engine;

auto OnEngineEvent() -> EngineEventObservable;
#define DEFINE_ON_EVENT(Name)                                                  \
  static inline auto On##Name##Event()->Name##EventObservable {                \
    return OnEngineEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
FOR_EACH_ENGINE_EVENT(DEFINE_ON_EVENT);
#undef DEFINE_ON_EVENT

static constexpr const auto kTargetTicksPerSecond = 120;
static constexpr const auto kTargetTickRate = NSEC_PER_SEC / kTargetTicksPerSecond;
class Engine {
  friend class prt::LuaState;
  friend class EngineState;
  friend class InitState;
  friend class ErrorState;
  friend class PausedState;
  friend class RunningState;
  friend class TerminatingState;
  friend class EngineTicker;
  friend class TerminatedState;
  DEFINE_NON_COPYABLE_TYPE(Engine);

 private:
  static void PublishEvent(EngineEvent* event);

  template <class E, typename... Args>
  static inline void Publish(Args... args) {
    E event(args...);
    return PublishEvent(&event);
  }

 private:
  uv::Loop loop_;
  RateLimitedTicker<kTargetTickRate> ticker_;
#ifdef PRT_DEBUG
  TickProfiler tick_profiler_;
#endif  // PRT_DEBUG
  rx::subscription on_tick_;
  EngineStatePtr state_{};
  std::shared_ptr<CrashReportCause> cause_ = nullptr;
  uv::Async on_shutdown_;

  void EnterState(const EngineStatePtr& state);
  void ExitState(const EngineStatePtr& state);

  template <class S, typename... Args>
  inline void SetState(Args... args) {
    on_tick_.unsubscribe();
    if (state_)
      ExitState(state_);
    state_ = S::New(args...);
    ASSERT(state_);
    EnterState(state_);
    on_tick_ = OnTick().subscribe([this](const Tick& tick) {
      ASSERT(state_);
      state_->OnTick(this, tick, GetPreviousTick());
    });
  }

  void Terminate();

  void Stop() {
    ticker_.Stop();
    loop_.Stop();
  }

  static void OnShutdown(uv_async_t* handle);

 public:
  Engine();
  ~Engine();

  auto GetLoop() const -> const uv::Loop& {
    return loop_;
  }

  auto GetLoop() -> uv::Loop& {
    return loop_;
  }

  auto Run() -> int;
  void Shutdown(std::shared_ptr<CrashReportCause> cause);

  inline void Shutdown(const std::exception_ptr& cause = nullptr, const int depth = CrashReportCause::kDefaultDepth,
                       const int offset = CrashReportCause::kDefaultOffset) {
    if (cause)
      return Shutdown(CrashReportCause::New(cause, depth, offset));
    return Shutdown((std::shared_ptr<CrashReportCause>)nullptr);
  }

  inline auto HasState() const -> bool {
    return state_ != nullptr;
  }

  auto GetState() const -> const std::unique_ptr<EngineState>& {
    return state_;
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

 private:
  static void InitLua(lua_State* L);

 public:
  static void Init();
  static auto Get() -> Engine*;
};
}  // namespace prt

#endif  // PRT_ENGINE_H
