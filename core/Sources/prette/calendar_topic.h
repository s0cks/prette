#ifndef PRT_CALENDAR_TOPIC_H
#define PRT_CALENDAR_TOPIC_H

#include <type_traits>

#include "prette/assertions.h"
#include "prette/calendar_event.h"
#include "prette/common.h"

namespace prt {
#define FOR_EACH_CALENDAR_TOPIC_EVENT(V) FOR_EACH_CALENDAR_EVENT(V)

template <class E>
struct is_calendar_topic_event_t : std::false_type {};

#define DEFINE_IS_CALENDAR_TOPIC_EVENT(Name) \
  template <>                                \
  struct is_calendar_topic_event_t<Name##Event> : std::true_type {};
FOR_EACH_CALENDAR_TOPIC_EVENT(DEFINE_IS_CALENDAR_TOPIC_EVENT)
#undef DEFINE_IS_CALENDAR_TOPIC_EVENT

template <class E>
concept CalendarTopicEvent = is_calendar_topic_event_t<E>::value;

class CalendarTopic {
  DEFINE_DEFAULT_COPYABLE_TYPE(CalendarTopic);

 private:
  NewDayEventSubject day_{};
  NewMonthEventSubject month_{};
  NewYearEventSubject year_{};
  NewSeasonEventSubject season_{};

 public:
  CalendarTopic() = default;
  ~CalendarTopic() = default;

  template <CalendarTopicEvent E>
  inline auto GetObservable() const -> rx::observable<E*>;

  template <CalendarTopicEvent E, typename... ArgN>
  inline auto On(ArgN... args) -> rx::composite_subscription {
    return GetObservable<E>().subscribe(args...);
  }

  template <CalendarTopicEvent E>
  inline void Publish(E* event);

#define DECLARE_PUBLISH(Name) \
  template <typename... Args> \
  inline void Publish##Name##Event(Args... args);
  FOR_EACH_CALENDAR_TOPIC_EVENT(DECLARE_PUBLISH)
#undef DECLARE_PUBLISH
};

#define DEFINE_GET_OBSERVABLE(Name, Subject)                                                      \
  template <>                                                                                     \
  inline auto CalendarTopic::GetObservable<Name##Event>() const -> rx::observable<Name##Event*> { \
    return (Subject).get_observable();                                                            \
  }
DEFINE_GET_OBSERVABLE(NewDay, day_);
DEFINE_GET_OBSERVABLE(NewMonth, month_);
DEFINE_GET_OBSERVABLE(NewYear, year_);
DEFINE_GET_OBSERVABLE(NewSeason, season_);
#undef DEFINE_GET_OBSERVABLE

#define DEFINE_PUBLISH(Name, Subject)                                    \
  template <>                                                            \
  inline void CalendarTopic::Publish<Name##Event>(Name##Event * event) { \
    ASSERT(event);                                                       \
    const auto& subscriber = (Subject).get_subscriber();                 \
    return subscriber.on_next(event);                                    \
  }
DEFINE_PUBLISH(NewDay, day_);
DEFINE_PUBLISH(NewMonth, month_);
DEFINE_PUBLISH(NewYear, year_);
DEFINE_PUBLISH(NewSeason, season_);
#undef DEFINE_PUBLISH

#define DECLARE_PUBLISH(Name)                                     \
  template <typename... Args>                                     \
  inline void CalendarTopic::Publish##Name##Event(Args... args) { \
    Name##Event event(args...);                                   \
    return Publish(&event);                                       \
  }
FOR_EACH_CALENDAR_TOPIC_EVENT(DECLARE_PUBLISH)
#undef DECLARE_PUBLISH
}  // namespace prt

#endif  // PRT_CALENDAR_TOPIC_H
