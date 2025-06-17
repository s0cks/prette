#ifndef PRT_TIME_EVENT_H
#define PRT_TIME_EVENT_H

#include "prette/assertions.h"
#include "prette/event.h"

namespace prt {
#define FOR_EACH_TIME_EVENT(V) \
  V(NewDay)                    \
  V(NewMonth)                  \
  V(NewYear)

class TimeEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_TIME_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

class TimeEvent : public Event {
 protected:
  TimeEvent() = default;

 public:
  ~TimeEvent() override = default;
  DEFINE_EVENT_PROTOTYPE_TYPE(Time, FOR_EACH_TIME_EVENT);

 public:
  static inline auto Cast(TimeEvent* event) -> TimeEvent* {
    ASSERT(event);
    return event;
  }
};

#define DECLARE_TIME_EVENT(Name)           \
  class Name##Event : public TimeEvent {   \
   public:                                 \
    Name##Event() = default;               \
    ~Name##Event() override = default;     \
    DECLARE_EVENT_TYPE(Time##Event, Name); \
  };
FOR_EACH_TIME_EVENT(DECLARE_TIME_EVENT)
#undef DECLARE_TIME_EVENT

DEFINE_EVENT_SUBJECT(Time);
DEFINE_EVENT_OBSERVABLE(Time);
FOR_EACH_TIME_EVENT(DEFINE_EVENT_SUBJECT);
FOR_EACH_TIME_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_TIME_EVENT_H
