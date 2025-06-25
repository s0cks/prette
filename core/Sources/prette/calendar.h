#ifndef PRT_CALENDAR_H
#define PRT_CALENDAR_H

#include "prette/calendar_state.h"
#include "prette/calendar_topic.h"
#include "prette/common.h"
#include "prette/season.h"
#include "prette/tick.h"

namespace prt {
class Calendar {
  friend class City;
  DEFINE_DEFAULT_COPYABLE_TYPE(Calendar);

 private:
  CalendarTopic topic_{};
  CalendarState state_{};
  Season season_ = kWinter;

  void Update(const TickDelta delta);

  inline auto state_ptr() -> CalendarState* {
    return &state_;
  }

  inline void NewDay() {
    state_.hour = 0;
    state_.day += 1;
    return topic_.PublishNewDayEvent(GetDay());
  }

  inline void NewMonth() {
    state_.day = 0;
    state_.month += 1;
    return topic_.PublishNewMonthEvent(GetMonth());
  }

  inline void NewYear() {
    state_.month = 0;
    state_.year += 1;
    topic_.PublishNewYearEvent(GetYear());
  }

  inline void NewSeason() {
    topic_.PublishNewSeasonEvent(GetSeason());
  }

 public:
  Calendar() = default;
  ~Calendar() = default;

  auto GetTopic() const -> const CalendarTopic& {
    return topic_;
  }

  auto GetTopic() -> CalendarTopic& {
    return topic_;
  }

  inline auto GetHour() const -> CalendarHour {
    return state_.hour;
  }

  inline auto GetDay() const -> CalendarDay {
    return state_.day;
  }

  inline auto GetMonth() const -> CalendarMonth {
    return state_.month;
  }

  inline auto GetYear() const -> CalendarYear {
    return state_.year;
  }

  inline auto GetSeason() const -> Season {
    return season_;
  }
};
}  // namespace prt

#endif  // PRT_CALENDAR_H
