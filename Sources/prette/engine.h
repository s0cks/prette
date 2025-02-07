#ifndef PRT_ENGINE_H
#define PRT_ENGINE_H

#include <units.h>
#include <uv.h>

#include "prette/common.h"
#include "prette/crash_report.h"
#include "prette/event.h"
#include "prette/ticker.h"
#include "prette/uv/utils.h"

namespace prt::engine {
#define FOR_EACH_ENGINE_EVENT(V) \
  V(PreInit)                     \
  V(PostInit)                    \
  V(PreTick)                     \
  V(Tick)                        \
  V(PostTick)                    \
  V(Terminating)                 \
  V(Terminated)

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

  DEFINE_EVENT_PROTOTYPE(FOR_EACH_ENGINE_EVENT);
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
  auto On##Name() const->Name##EventObservable {                         \
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
  V(Terminating)                 \
  V(Error)

class Engine;
class EngineState;
#define FORWARD_DECLARE_STATE(Name) class Name##State;
FOR_EACH_ENGINE_STATE(FORWARD_DECLARE_STATE)
#undef FORWARD_DECLARE_STATE

#define DECLARE_ENGINE_STATE_TYPE(Name)               \
 protected:                                           \
  void Execute(Engine* engine) override;              \
                                                      \
 public:                                              \
  auto GetStateName() const -> const char* override { \
    return #Name;                                     \
  }                                                   \
  auto As##Name##State()->Name##State* override {     \
    return this;                                      \
  }

#define DECLARE_ENGINE_STATE(Name)         \
  class Name##State : public EngineState { \
   public:                                 \
    explicit Name##State(Engine* engine) : \
      EngineState(engine) {}               \
    ~Name##State() override = default;     \
    DECLARE_ENGINE_STATE_TYPE(Name);       \
  };

class EngineState {
  friend class Engine;
  DEFINE_NON_COPYABLE_TYPE(EngineState);

 public:
  using DurationSeries = TimeSeries<>;

 private:
  Engine* engine_;
  DurationSeries duration_{};

 protected:
  explicit EngineState(Engine* engine) :
    engine_(engine) {
    ASSERT(engine_);
  }

  virtual void Stop() {
    // do nothing
  }

  virtual void Execute(Engine* engine) = 0;

 public:
  virtual ~EngineState() = default;
  virtual auto GetStateName() const -> const char* = 0;

  auto GetDurationSeries() const -> const DurationSeries& {
    return duration_;
  }

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
  Ticker ticker_;
  rx::subscription on_tick_{};

  explicit RunningState(Engine* engine);

 public:
  ~RunningState() override;

  DECLARE_ENGINE_STATE_TYPE(Running);

 public:
  static inline auto New(Engine* engine) -> RunningState* {
    ASSERT(engine);
    return new RunningState(engine);
  }
};

DECLARE_ENGINE_STATE(Paused);
DECLARE_ENGINE_STATE(Terminating);

class ErrorState : public EngineState {
 private:
  std::exception_ptr cause_;

  explicit ErrorState(Engine* engine, const std::exception_ptr cause) :
    EngineState(engine),
    cause_(cause) {}

 public:
  ~ErrorState() override = default;

  auto GetCause() const -> std::exception_ptr {
    return cause_;
  }

  DECLARE_ENGINE_STATE_TYPE(Error);

 public:
  static inline auto New(Engine* engine, const std::exception_ptr& cause) -> ErrorState* {
    ASSERT(engine);
    return new ErrorState(engine, cause);
  }
};

#define ENGINE_STATE_F(Name) void Name##State::Execute(Engine* engine)

class Engine : public EngineEventSource {
  friend class EngineState;
  friend class InitState;
  friend class ErrorState;
  friend class RunningState;
  friend class EngineTicker;
  friend class TerminatedState;
  DEFINE_NON_COPYABLE_TYPE(Engine);

 private:
  uv::Loop loop_;
  rx::subscription on_tick_;
  RelaxedAtomic<bool> running_;
  EngineState* state_;
  EngineEventSubject events_;

  virtual void SetRunning(const bool running = true) {
    running_ = running;
  }

  inline void SetState(EngineState* state) {
    ASSERT(state);
    state_ = state;
  }

  void RunState(EngineState* state);

  template <class S, typename... Args>
  inline void RunState(Args... args) {
    S state(this, args...);
    return RunState(&state);
  }

  void Terminate() {
    GetLoop().Stop();
  }

  void PublishEvent(EngineEvent* event) const override;
  auto TransitionTo(EngineState* state) -> bool;
  auto CancelTransitionTo(EngineState* new_state) -> bool;

  auto Error(const std::exception_ptr& cause) -> bool {
    return TransitionTo(ErrorState::New(this, cause));
  }

#define DEFINE_PUBLISH_EVENT(Name)           \
  inline void Publish##Name##Event() const { \
    return Publish<Name##Event>(this);       \
  }
  DEFINE_PUBLISH_EVENT(PreInit);
  DEFINE_PUBLISH_EVENT(PostInit);
  DEFINE_PUBLISH_EVENT(Terminating);
#undef DEFINE_PUBLISH_EVENT

 public:
  Engine();
  ~Engine() override;

  auto GetLoop() const -> const uv::Loop& {
    return loop_;
  }

  auto GetLoop() -> uv::Loop& {
    return loop_;
  }

  void Run();
  void Shutdown(const std::exception_ptr& cause = nullptr);

  template <typename E, typename... Args>
  inline void Shutdown(Args... args) {
    return Shutdown((const std::exception_ptr&)std::make_exception_ptr<E>(E(args...)));
  }

  virtual auto IsRunning() const -> bool {
    return (bool)running_;
  }

  inline auto HasState() const -> bool {
    return state_ != nullptr;
  }

  virtual auto GetState() const -> EngineState* {
    return state_;
  }

  auto OnEvent() const -> EngineEventObservable override;
};

void InitEngine();
auto GetEngine() -> Engine*;
}  // namespace prt::engine

namespace prt {
using engine::Engine;
using engine::GetEngine;
}  // namespace prt

#endif  // PRT_ENGINE_H
