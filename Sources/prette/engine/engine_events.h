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

  class EngineEvent;
  class StateEvent;
#define FORWARD_DECLARE_ENGINE_EVENT(Name) class Name##Event;
  FOR_EACH_ENGINE_EVENT(FORWARD_DECLARE_ENGINE_EVENT)
#undef FORWARD_DECLARE_ENGINE_EVENT

  class Engine;
  class EngineEvent : public Event {
  protected:
    Engine* engine_;

    explicit EngineEvent(Engine* engine):
      Event(),
      engine_(engine) {
    }
  public:
    virtual ~EngineEvent() = default;

    Engine* engine() const {
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
  protected:
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

    const uv::Tick& GetCurrentTick() const {
      return current_;
    }

    const uv::Tick& GetPreviousTick() const {
      return previous_;
    }

    uv::TickDelta GetTimeSinceLast() const {
      return GetCurrentTick() - GetPreviousTick();
    }

    DECLARE_ENGINE_EVENT(Tick);
  };

  class PostTickEvent : public EngineEvent {
  public:
    uv::Tick tick_;

    explicit PostTickEvent(Engine* engine, const uv::Tick& tick):
      EngineEvent(engine),
      tick_(tick) {
    }
    ~PostTickEvent() override = default;

    const uv::Tick& tick() const {
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

  typedef rx::subject<EngineEvent*> EngineEventSubject;

  class EngineEventPublisher : public EventPublisher<EngineEvent> {
  protected:
    EngineEventPublisher() = default;
  public:
    ~EngineEventPublisher() override = default;
  };
}

#endif //PRT_ENGINE_EVENT_H