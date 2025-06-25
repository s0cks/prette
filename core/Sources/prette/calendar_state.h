#ifndef PRT_CALENDAR_STATE_H
#define PRT_CALENDAR_STATE_H

#include <cstdint>
#include <glog/logging.h>

namespace prt {
using CalendarHour = uint8_t;
static constexpr const CalendarHour kCalendarHourMax = UINT8_MAX;

using CalendarDay = uint8_t;
static constexpr const CalendarDay kCalendarDayMax = UINT8_MAX;

using CalendarMonth = uint8_t;
static constexpr const CalendarMonth kCalendarMonthMax = UINT8_MAX;

using CalendarYear = uint32_t;
static constexpr const CalendarYear kCalendarYearMax = UINT8_MAX;

static constexpr const auto kMaxNumberOfHoursPerDay = 24;

struct alignas(4) CalendarState {
  CalendarHour hour{};
  CalendarDay day{};
  CalendarMonth month{};
  CalendarYear year{};

  inline constexpr auto IsNewDay() const -> bool {
    return hour >= kMaxNumberOfHoursPerDay;
  }

  inline constexpr auto IsNewMonth() const -> bool {
    if (month == 1) [[unlikely]]
      return day >= 27;
    else if ((month % 2) == 0)
      return day >= 30;
    else [[likely]]
      return day >= 31;
  }

  inline constexpr auto IsNewYear() const -> bool {
    return month >= 12;
  }

  inline constexpr auto operator==(const CalendarState& rhs) const -> bool {
    return hour == rhs.hour && day == rhs.day && month == rhs.month && year == rhs.year;
  }

  inline constexpr auto operator!=(const CalendarState& rhs) const -> bool {
    return hour != rhs.hour || day != rhs.day || month != rhs.month || year != rhs.year;
  }
};

static constexpr const CalendarHour kAnyCalendarHour = kCalendarHourMax;
static constexpr const CalendarDay kAnyCalendarDay = kCalendarDayMax;
static constexpr const CalendarMonth kAnyCalendarMonth = kCalendarMonthMax;
static constexpr const CalendarDay kAnyCalendarYear = kCalendarYearMax;

struct alignas(4) CalendarStatePattern {
  CalendarHour hour = kAnyCalendarHour;
  CalendarDay day = kAnyCalendarDay;
  CalendarMonth month = kAnyCalendarMonth;
  CalendarYear year = kAnyCalendarYear;

  inline auto constexpr operator==(const CalendarState& rhs) const -> bool {
    // clang-format off
    return (hour == kAnyCalendarHour || hour == rhs.hour)
      && (day == kAnyCalendarDay || day == rhs.day)
      && (month == kAnyCalendarMonth || month == rhs.month)
      && (year == kAnyCalendarYear || year == rhs.year);
    // clang-format on
  }

  inline auto constexpr operator!=(const CalendarState& rhs) const -> bool {
    // clang-format off
    return (hour == kAnyCalendarHour || hour != rhs.hour)
        || (day == kAnyCalendarDay || day != rhs.day)
        || (month == kAnyCalendarMonth || month != rhs.month)
        || (year == kAnyCalendarYear || year != rhs.year);
    // clang-format on
  }
};
}  // namespace prt

#endif  // PRT_CALENDAR_STATE_H
