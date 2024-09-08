#ifndef PRT_ENGINE_EVENT_H
#define PRT_ENGINE_EVENT_H

#include "prette/rx.h"
#include "prette/event.h"
#include "prette/uv/uv_tick.h"

namespace prt::engine {
#define FOR_EACH_ENGINE_EVENT(V) \
  V(PreInit)                     \
  V(PostInit)                    \
  V(PreTick)                     \
  V(Tick)                        \
  V(PostTick)                    \
  V(Terminating)                 \
  V(Terminated)                  \
  V(Initialized)

  class Engine;
  class EngineEvent;
  class StateEvent;
#define FORWARD_DECLARE_ENGINE_EVENT(Name) class Name##Event;
  FOR_EACH_ENGINE_EVENT(FORWARD_DECLARE_ENGINE_EVENT)
#undef FORWARD_DECLARE_ENGINE_EVENT

  class EngineEvent : public Event {
    DEFINE_NON_COPYABLE_TYPE(EngineEvent);
  private:
    Engine* engine_;
  protected:
    explicit EngineEvent(Engine* engine):
      Event(),
      engine_(engine) {
    }
  public:
    ~EngineEvent() override = default;

    auto GetEngine() const -> Engine* {
      return engine_;
    }

    DEFINE_EVENT_PROTOTYPE(FOR_EACH_ENGINE_EVENT);
  };

#define DECLARE_ENGINE_EVENT(Name)        \
  DECLARE_EVENT_TYPE(EngineEvent, Name)

  class PreInitEvent : public EngineEvent {
  public:
    explicit PreInitEvent(Engine* engine):
      EngineEvent(engine) {
    }
    ~PreInitEvent() override = default;
    DECLARE_ENGINE_EVENT(PreInit);
  };

  class PostInitEvent : public EngineEvent {
  public:
    explicit PostInitEvent(Engine* engine):
      EngineEvent(engine) {
    }
    ~PostInitEvent() override = default;
    DECLARE_ENGINE_EVENT(PostInit);
  };

  class PreTickEvent : public EngineEvent {
  public:
    explicit PreTickEvent(Engine* engine):
      EngineEvent(engine) {
    }
    ~PreTickEvent() override = default;
    DECLARE_ENGINE_EVENT(PreTick);
  };

  class TickEvent : public EngineEvent {
  private:
    const uv::Tick& current_;
    const uv::Tick& previous_;
  public:
    explicit TickEvent(Engine* engine,
                       const uv::Tick& current,
                       const uv::Tick& previous):
      EngineEvent(engine),
      current_(current),
      previous_(previous) {
    }
    ~TickEvent() override = default;

    auto GetCurrentTick() const -> const uv::Tick& {
      return current_;
    }

    auto GetPreviousTick() const -> const uv::Tick& {
      return previous_;
    }

    auto GetTimeSinceLast() const -> uv::TickDelta {
      return GetCurrentTick() - GetPreviousTick();
    }

    DECLARE_ENGINE_EVENT(Tick);
  };

  class PostTickEvent : public EngineEvent {
  private:
    uv::Tick tick_;
  public:
    explicit PostTickEvent(Engine* engine, const uv::Tick& tick):
      EngineEvent(engine),
      tick_(tick) {
    }
    ~PostTickEvent() override = default;

    auto GetTick() const -> const uv::Tick& {
      return tick_;
    }

    DECLARE_ENGINE_EVENT(PostTick);
  };

  class TerminatingEvent : public EngineEvent {
  public:
    explicit TerminatingEvent(Engine* engine):
      EngineEvent(engine) {
    }
    ~TerminatingEvent() override = default;
    DECLARE_ENGINE_EVENT(Terminating);
  };

  class TerminatedEvent : public EngineEvent {
  public:
    explicit TerminatedEvent(Engine* engine):
      EngineEvent(engine) {
    }
    ~TerminatedEvent() override = default;
    DECLARE_ENGINE_EVENT(Terminated);
  };

  class InitializedEvent : public EngineEvent {
  public:
    explicit InitializedEvent(Engine* engine):
      EngineEvent(engine) {
    }
    ~InitializedEvent() override = default;
    DECLARE_ENGINE_EVENT(Initialized);
  };

  using EngineEventSubject = rx::subject<EngineEvent*>;
  using EngineEventObservable = rx::observable<EngineEvent*>;
#define DEFINE_EVENT_OBSERVABLE(Name)   \
  using Name##EventObservable = rx::observable<Name##Event*>;
  FOR_EACH_ENGINE_EVENT(DEFINE_EVENT_OBSERVABLE)
#undef DEFINE_EVENT_OBSERVABLE

  class EngineEventSource : public EventSource<EngineEvent> {
    DEFINE_NON_COPYABLE_TYPE(EngineEventSource);
  protected:
    EngineEventSource() = default;
  public:
    ~EngineEventSource() override = default;
#define DEFINE_ON_EVENT(Name)   \
    auto On##Name() const -> Name##EventObservable {    \
      return OnEvent()                                  \
        .filter(Name##Event::Filter)                    \
        .map(Name##Event::Cast);                        \
    }
    FOR_EACH_ENGINE_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
  };
}

#endif //PRT_ENGINE_EVENT_H