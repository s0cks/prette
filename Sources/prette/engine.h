#ifndef PRT_ENGINE_H
#define PRT_ENGINE_H

#include <units.h>
#include <uv.h>

#include <memory>

#include "prette/common.h"
#include "prette/crash_report.h"
#include "prette/event.h"
#include "prette/lua.h"
#include "prette/tick.h"
#include "prette/tick_profiler.h"
#include "prette/ticker.h"
#include "prette/uv/utils.h"

namespace prt {
class LuaState;
}

namespace prt::engine {
#define FOR_EACH_ENGINE_EVENT(V) \
  V(PreInit)                     \
  V(PostInit)                    \
  V(PreTick)                     \
  V(Tick)                        \
  V(PostTick)                    \
  V(Terminating)                 \
  V(Terminated)                  \
  V(Error)

class Engine;
class EngineEvent;
class StateEvent;
#define FORWARD_DECLARE_ENGINE_EVENT(Name) class Name##Event;
FOR_EACH_ENGINE_EVENT(FORWARD_DECLARE_ENGINE_EVENT)
#undef FORWARD_DECLARE_ENGINE_EVENT

class EngineEvent : public Event {
  DEFINE_NON_COPYABLE_TYPE(EngineEvent);

 private:
  const Engine* engine_;

 protected:
  explicit EngineEvent(const Engine* engine) :
    Event(),
    engine_(engine) {}

 public:
  ~EngineEvent() override = default;

  auto GetEngine() const -> const Engine* {
    return engine_;
  }

  DEFINE_EVENT_PROTOTYPE(Engine, FOR_EACH_ENGINE_EVENT);
};

#define DECLARE_ENGINE_EVENT(Name) DECLARE_EVENT_TYPE(EngineEvent, Name)

class PreInitEvent : public EngineEvent {
 public:
  explicit PreInitEvent(const Engine* engine) :
    EngineEvent(engine) {}
  ~PreInitEvent() override = default;
  DECLARE_ENGINE_EVENT(PreInit);
};

class PostInitEvent : public EngineEvent {
 public:
  explicit PostInitEvent(const Engine* engine) :
    EngineEvent(engine) {}
  ~PostInitEvent() override = default;
  DECLARE_ENGINE_EVENT(PostInit);
};

class PreTickEvent : public EngineEvent {
 public:
  explicit PreTickEvent(const Engine* engine) :
    EngineEvent(engine) {}
  ~PreTickEvent() override = default;
  DECLARE_ENGINE_EVENT(PreTick);
};

class TickEvent : public EngineEvent {
 private:
  const Tick& current_;
  const Tick& previous_;

 public:
  TickEvent(const Engine* engine, const Tick& current, const Tick& previous) :
    EngineEvent(engine),
    current_(current),
    previous_(previous) {}
  TickEvent(const Engine* engine, const Tick& current) :
    TickEvent(engine, current, current) {}
  ~TickEvent() override = default;

  auto GetCurrentTick() const -> const Tick& {
    return current_;
  }

  auto GetPreviousTick() const -> const Tick& {
    return previous_;
  }

  auto GetTimeSinceLast() const -> TickDelta {
    return GetCurrentTick() - GetPreviousTick();
  }

  DECLARE_ENGINE_EVENT(Tick);
};

class PostTickEvent : public EngineEvent {
 private:
  Tick tick_;

 public:
  explicit PostTickEvent(const Engine* engine, const Tick& tick) :
    EngineEvent(engine),
    tick_(tick) {}
  ~PostTickEvent() override = default;

  auto GetTick() const -> const Tick& {
    return tick_;
  }

  DECLARE_ENGINE_EVENT(PostTick);
};

class TerminatingEvent : public EngineEvent {
 public:
  explicit TerminatingEvent(const Engine* engine) :
    EngineEvent(engine) {}
  ~TerminatingEvent() override = default;
  DECLARE_ENGINE_EVENT(Terminating);
};

class TerminatedEvent : public EngineEvent {
 public:
  explicit TerminatedEvent(const Engine* engine) :
    EngineEvent(engine) {}
  ~TerminatedEvent() override = default;
  DECLARE_ENGINE_EVENT(Terminated);
};

class ErrorEvent : public EngineEvent {
 public:
  explicit ErrorEvent(const Engine* engine) :
    EngineEvent(engine) {}
  ~ErrorEvent() override = default;
  DECLARE_ENGINE_EVENT(Error);
};

DEFINE_EVENT_SUBJECT(Engine);
DEFINE_EVENT_OBSERVABLE(Engine);
FOR_EACH_ENGINE_EVENT(DEFINE_EVENT_OBSERVABLE);

class EngineEventSource : public EventSource<EngineEvent> {
  DEFINE_NON_COPYABLE_TYPE(EngineEventSource);

 protected:
  EngineEventSource() = default;

 public:
  ~EngineEventSource() override = default;
#define DEFINE_ON_EVENT(Name)                                            \
  auto On##Name##Event() const->Name##EventObservable {                  \
    return OnEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
  FOR_EACH_ENGINE_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
};

class Engine;

#define FOR_EACH_ENGINE_STATE(V) \
  V(Init)                        \
  V(Running)                     \
  V(Paused)                      \
  V(Terminated)                  \
  V(Error)

class Engine;
class EngineState;
#define FORWARD_DECLARE_STATE(Name) class Name##State;
FOR_EACH_ENGINE_STATE(FORWARD_DECLARE_STATE)
#undef FORWARD_DECLARE_STATE

#define DECLARE_ENGINE_STATE_TYPE(Name)                                                    \
 protected:                                                                                \
  virtual void EnterState(Engine* engine) override;                                        \
  virtual void OnTick(Engine* engine, const Tick& current, const Tick& previous) override; \
  virtual void ExitState(Engine* engine) override;                                         \
                                                                                           \
 public:                                                                                   \
  auto GetStateName() const -> const char* override {                                      \
    return #Name;                                                                          \
  }                                                                                        \
  auto As##Name##State()->Name##State* override {                                          \
    return this;                                                                           \
  }

#define DECLARE_ENGINE_STATE(Name)                             \
  class Name##State : public EngineState {                     \
   public:                                                     \
    Name##State() = default;                                   \
    ~Name##State() override = default;                         \
    DECLARE_ENGINE_STATE_TYPE(Name);                           \
                                                               \
   public:                                                     \
    static inline auto New() -> std::unique_ptr<Name##State> { \
      return std::make_unique<Name##State>();                  \
    }                                                          \
  };

class EngineState {
  friend class Engine;
  DEFINE_NON_COPYABLE_TYPE(EngineState);

 public:
  using DurationSeries = TimeSeries<>;

 protected:
  EngineState() = default;
  virtual void EnterState(Engine* engine) = 0;
  virtual void OnTick(Engine* engine, const Tick& current, const Tick& previous) = 0;
  virtual void ExitState(Engine* engine) = 0;

 public:
  virtual ~EngineState() = default;
  virtual auto GetStateName() const -> const char* = 0;

  virtual auto Equals(EngineState* rhs) const -> bool {
    ASSERT(rhs);
    return strcmp(GetStateName(), rhs->GetStateName()) == 0;
  }

#define DEFINE_TYPE_CHECK(Name)                  \
  virtual auto As##Name##State()->Name##State* { \
    return nullptr;                              \
  }                                              \
  auto Is##Name##State()->bool {                 \
    return As##Name##State() != nullptr;         \
  }
  FOR_EACH_ENGINE_STATE(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK
};

DECLARE_ENGINE_STATE(Init);

class RunningState : public EngineState {
  friend class Engine;

 private:
  rx::subscription on_tick_{};

 public:
  RunningState() = default;
  ~RunningState() override = default;

  DECLARE_ENGINE_STATE_TYPE(Running);

 public:
  static inline auto New() -> std::unique_ptr<RunningState> {
    return std::make_unique<RunningState>();
  }
};

DECLARE_ENGINE_STATE(Paused);
DECLARE_ENGINE_STATE(Terminated);

class ErrorState : public EngineState {
 private:
  std::shared_ptr<CrashReportCause> cause_;

 public:
  explicit ErrorState(const std::shared_ptr<CrashReportCause>& cause) :
    EngineState(),
    cause_(cause) {}
  ~ErrorState() override = default;

  auto GetCause() const -> const std::shared_ptr<CrashReportCause>& {
    return cause_;
  }

  DECLARE_ENGINE_STATE_TYPE(Error);

 public:
  static inline auto New(const std::shared_ptr<CrashReportCause>& cause) -> std::unique_ptr<ErrorState> {
    return std::make_unique<ErrorState>(cause);
  }
};

#define ENGINE_STATE_ENTER_F(Name) void Name##State::EnterState(Engine* engine)
#define ENGINE_STATE_TICK_F(Name)  void Name##State::OnTick(Engine* engine, const Tick& current, const Tick& previous)
#define ENGINE_STATE_EXIT_F(Name)  void Name##State::ExitState(Engine* engine)

class Engine : public EngineEventSource {
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
  uv::Loop loop_;
  uv::Async on_shutdown_;
  EngineEventSubject events_;
  RateLimitedTicker<50000000> ticker_;
#ifdef PRT_DEBUG
  TickProfiler tick_profiler_;
#endif  // PRT_DEBUG
  rx::subscription on_tick_;
  std::unique_ptr<EngineState> state_{};
  std::shared_ptr<CrashReportCause> cause_ = nullptr;

  void EnterState(const std::unique_ptr<EngineState>& state);
  void ExitState(const std::unique_ptr<EngineState>& state);

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

  void PublishEvent(EngineEvent* event) const override;
  void Terminate();

#define DEFINE_PUBLISH_EVENT(Name)           \
  inline void Publish##Name##Event() const { \
    return Publish<Name##Event>(this);       \
  }
  DEFINE_PUBLISH_EVENT(PreInit);
  DEFINE_PUBLISH_EVENT(PostInit);
  DEFINE_PUBLISH_EVENT(Terminated);
  DEFINE_PUBLISH_EVENT(Terminating);
  DEFINE_PUBLISH_EVENT(Error);
#undef DEFINE_PUBLISH_EVENT

  void Stop() {
    ticker_.Stop();
    loop_.Stop();
  }

  static void OnShutdown(uv_async_t* handle);

 public:
  Engine();
  ~Engine() override;

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

  auto OnEvent() const -> EngineEventObservable override;

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
}  // namespace prt::engine

namespace prt {
using engine::Engine;
}  // namespace prt

#endif  // PRT_ENGINE_H
