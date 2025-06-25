#ifndef PRT_CALENDAR_EVENT_H
#define PRT_CALENDAR_EVENT_H

#include "prette/assertions.h"
#include "prette/calendar_state.h"
#include "prette/event.h"
#include "prette/season.h"

namespace prt {
#define FOR_EACH_CALENDAR_EVENT(V) \
  V(NewDay)                        \
  V(NewMonth)                      \
  V(NewYear)                       \
  V(NewSeason)

class CalendarEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_CALENDAR_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

class CalendarEvent : public Event {
 protected:
  CalendarEvent() = default;

 public:
  ~CalendarEvent() override = default;
  DEFINE_EVENT_PROTOTYPE_TYPE(Calendar, FOR_EACH_CALENDAR_EVENT);

 public:
  static inline auto Cast(CalendarEvent* event) -> CalendarEvent* {
    ASSERT(event);
    return event;
  }
};

#define DECLARE_CALENDAR_EVENT(Name)         \
  class Name##Event : public CalendarEvent { \
   public:                                   \
    Name##Event() = default;                 \
    ~Name##Event() override = default;       \
    DECLARE_EVENT_TYPE(CalendarEvent, Name); \
  };

class NewDayEvent : public CalendarEvent {
 private:
  CalendarDay day_;

 public:
  explicit NewDayEvent(const CalendarDay day) :
    CalendarEvent(),
    day_(day) {}
  ~NewDayEvent() override = default;

  auto GetDay() const -> CalendarDay {
    return day_;
  }

  DECLARE_EVENT_TYPE(CalendarEvent, NewDay);
};

class NewMonthEvent : public CalendarEvent {
 private:
  CalendarMonth month_;

 public:
  explicit NewMonthEvent(const CalendarMonth month) :
    CalendarEvent(),
    month_(month) {}
  ~NewMonthEvent() override = default;

  auto GetMonth() const -> CalendarMonth {
    return month_;
  }

  DECLARE_EVENT_TYPE(CalendarEvent, NewMonth);
};

class NewYearEvent : public CalendarEvent {
 private:
  CalendarYear year_;

 public:
  explicit NewYearEvent(const CalendarYear year) :
    CalendarEvent(),
    year_(year) {}
  ~NewYearEvent() override = default;

  auto GetYear() const -> CalendarYear {
    return year_;
  }

  DECLARE_EVENT_TYPE(CalendarEvent, NewYear);
};

class NewSeasonEvent : public CalendarEvent {
 private:
  Season season_;

 public:
  explicit NewSeasonEvent(const Season season) :
    CalendarEvent(),
    season_(season) {}
  ~NewSeasonEvent() override = default;

  auto GetSeason() const -> Season {
    return season_;
  }

  DECLARE_EVENT_TYPE(CalendarEvent, NewSeason);
};

#undef DECLARE_CALENDAR_EVENT

DEFINE_EVENT_SUBJECT(Calendar);
DEFINE_EVENT_OBSERVABLE(Calendar);
FOR_EACH_CALENDAR_EVENT(DEFINE_EVENT_SUBJECT);
FOR_EACH_CALENDAR_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_CALENDAR_EVENT_H
