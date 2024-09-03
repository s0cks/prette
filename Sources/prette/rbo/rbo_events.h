#ifndef PRT_RBO_EVENTS_H
#define PRT_RBO_EVENTS_H

#include <functional>
#include "prette/event.h"
#include "prette/rbo/rbo_id.h"

namespace prt::rbo {
#define FOR_EACH_RBO_EVENT(V) \
  V(RboCreated)               \
  V(RboDestroyed)

  class Rbo;
  class RboEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_RBO_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  class RboEvent : public Event {
  protected:
    const Rbo* rbo_;

    explicit RboEvent(const Rbo* rbo):
      Event(),
      rbo_(rbo) {
    }
  public:
    ~RboEvent() override = default;

    const Rbo* GetRbo() const {
      return rbo_;
    }

    RboId GetRboId() const;
    DEFINE_EVENT_PROTOTYPE(FOR_EACH_RBO_EVENT);
  };

#define DECLARE_RBO_EVENT(Name)                       \
  DECLARE_EVENT_TYPE(RboEvent, Name)                  \
  static inline std::function<bool(RboEvent*)>        \
  FilterBy(const RboId id) {                          \
    return [id](RboEvent* event) {                    \
      return event                                    \
          && event->Is##Name##Event()                 \
          && event->GetRboId() == id;                 \
    };                                                \
  }

  class RboCreatedEvent : public RboEvent {
  public:
    explicit RboCreatedEvent(const Rbo* rbo):
      RboEvent(rbo) {
    }
    ~RboCreatedEvent() override = default;
    DECLARE_RBO_EVENT(RboCreated);
  };

  class RboDestroyedEvent : public RboEvent {
  public:
    explicit RboDestroyedEvent(const Rbo* rbo):
      RboEvent(rbo) {
    }
    ~RboDestroyedEvent() override = default;
    DECLARE_RBO_EVENT(RboDestroyed);
  };
}

#endif //PRT_RBO_EVENTS_H