#ifndef PRT_ENGINE_EVENT_H
#define PRT_ENGINE_EVENT_H

#include "prette/engine/engine_state.h"
#include "prette/event.h"
#include "prette/rx.h"  // IWYU pragma: keep
#include "prette/tick.h"

namespace prt {
#define FOR_EACH_ENGINE_EVENT(V) \
  V(PreInit)                     \
  V(PostInit)                    \
  V(PreTick)                     \
  V(Tick)                        \
  V(PostTick)                    \
  FOR_EACH_ENGINE_STATE(V)

class Engine;
class EngineEvent;
#define FORWARD_DECLARE_ENGINE_EVENT(Name) class Name##Event;
FOR_EACH_ENGINE_EVENT(FORWARD_DECLARE_ENGINE_EVENT)
#undef FORWARD_DECLARE_ENGINE_EVENT

DEFINE_EVENT_PROTOTYPE(Engine, FOR_EACH_ENGINE_EVENT);

#define DECLARE_ENGINE_EVENT_TYPE(Name) DECLARE_EVENT_TYPE(EngineEvent, Name)

#define DECLARE_ENGINE_EVENT(Name)         \
  class Name##Event : public EngineEvent { \
   public:                                 \
    Name##Event() = default;               \
    ~Name##Event() override = default;     \
    DECLARE_ENGINE_EVENT_TYPE(Name);       \
  };

DECLARE_ENGINE_EVENT(PreInit);
DECLARE_ENGINE_EVENT(PostInit);
DECLARE_ENGINE_EVENT(PreTick);

class TickEvent : public EngineEvent {
 private:
  Tick previous_;
  Tick current_;

 public:
  TickEvent(const Tick& current, const Tick& previous) :
    EngineEvent(),
    current_(current),
    previous_(previous) {}
  explicit TickEvent(const Tick& current) :
    TickEvent(current, current) {}
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

  DECLARE_ENGINE_EVENT_TYPE(Tick);
};

class PostTickEvent : public EngineEvent {
 private:
  Tick tick_;

 public:
  explicit PostTickEvent(const Tick& tick) :
    EngineEvent(),
    tick_(tick) {}
  ~PostTickEvent() override = default;

  auto GetTick() const -> const Tick& {
    return tick_;
  }

  DECLARE_ENGINE_EVENT_TYPE(PostTick);
};

FOR_EACH_ENGINE_STATE(DECLARE_ENGINE_EVENT);

DEFINE_EVENT_SUBJECT(Engine);
DEFINE_EVENT_OBSERVABLE(Engine);
FOR_EACH_ENGINE_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_ENGINE_EVENT_H
