#ifndef PRT_MONITOR_EVENTS_H
#define PRT_MONITOR_EVENTS_H

#include "prette/event.h"

namespace prt::window {
#define FOR_EACH_MONITOR_EVENT(V)   \
  V(MonitorConnected)               \
  V(MonitorDisconnected)

  class Monitor;
  class MonitorEvent;
#define DECLARE_MONITOR_EVENT(Name) class Name##Event;
  FOR_EACH_MONITOR_EVENT(DECLARE_MONITOR_EVENT);
#undef DECLARE_MONITOR_EVENT

  class MonitorEvent : public Event {
    DEFINE_NON_COPYABLE_TYPE(MonitorEvent);
  private:
    const Monitor* monitor_;
  protected:
    explicit MonitorEvent(const Monitor* monitor):
      Event(),
      monitor_(monitor) {
    }
  public:
    ~MonitorEvent() override = default;

    auto GetMonitor() const -> const Monitor* {
      return monitor_;
    }

    DEFINE_EVENT_PROTOTYPE(FOR_EACH_MONITOR_EVENT);
  };

#define DECLARE_MONITOR_EVENT(Name)     \
  DECLARE_EVENT_TYPE(MonitorEvent, Name)

  class MonitorConnectedEvent : public MonitorEvent {
  public:
    explicit MonitorConnectedEvent(const Monitor* monitor):
      MonitorEvent(monitor) {
    }
    ~MonitorConnectedEvent() override = default;
    DECLARE_MONITOR_EVENT(MonitorConnected);
  };

  class MonitorDisconnectedEvent : public MonitorEvent {
  public:
    explicit MonitorDisconnectedEvent(const Monitor* monitor):
      MonitorEvent(monitor) {
    }
    ~MonitorDisconnectedEvent() override = default;
    DECLARE_MONITOR_EVENT(MonitorDisconnected);
  };

  DEFINE_EVENT_SUBJECT(Monitor);
  DEFINE_EVENT_OBSERVABLE(Monitor);
  FOR_EACH_MONITOR_EVENT(DEFINE_EVENT_OBSERVABLE);

#undef DECLARE_MONITOR_EVENT

  class MonitorEventSource : public EventSource<MonitorEvent> {
    DEFINE_NON_COPYABLE_TYPE(MonitorEventSource);
  protected:
    MonitorEventSource() = default;
  public:
    ~MonitorEventSource() override = default;
#define DEFINE_ON_EVENT(Name)                                 \
    inline auto On##Name() const -> Name##EventObservable {   \
      return OnEvent()                                        \
        .filter(Name##Event::Filter)                          \
        .map(Name##Event::Cast);                              \
    }
    FOR_EACH_MONITOR_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
  };
}

#endif //PRT_MONITOR_EVENTS_H