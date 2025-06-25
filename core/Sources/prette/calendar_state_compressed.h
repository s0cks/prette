#ifndef PRT_CALENDAR_STATE_COMPRESSED_H
#define PRT_CALENDAR_STATE_COMPRESSED_H

#include "prette/bitfield.h"
#include "prette/calendar_state.h"
#include "prette/common.h"
#include "prette/platform.h"

namespace prt {

using RawCalendarState = uint64_t;
static constexpr const RawCalendarState kInvalidCalendarState = 0x0;
class CompressedCalendarState {
  DEFINE_DEFAULT_COPYABLE_TYPE(CompressedCalendarState);

 private:
  template <class T, const uint64_t Pos, const uint64_t Length>
  class StateField : public BitField<RawCalendarState, T, Pos, Length> {};

 public:
  enum Layout {
    kHourPos = 0,
    kBitsForHour = 8,

    kDayPos = kHourPos + kBitsForHour,
    kBitsForDay = 8,

    kMonthPos = kDayPos + kBitsForDay,
    kBitsForMonth = 8,

    kYearPos = kMonthPos + kBitsForMonth,
    kBitsForYear = 32,

    kTotalNumberOfBits = kBitsForHour + kBitsForDay + kBitsForYear,
  };
  static_assert(Layout::kTotalNumberOfBits <= kBitsPerWord, "expected CalendarState to fit in a word");
  class HourField : public StateField<CalendarHour, kHourPos, kBitsForHour> {};
  class DayField : public StateField<CalendarDay, kDayPos, kBitsForDay> {};
  class MonthField : public StateField<CalendarMonth, kMonthPos, kBitsForMonth> {};
  class YearField : public StateField<CalendarYear, kYearPos, kBitsForYear> {};

 private:
  RawCalendarState value_;

 public:
  constexpr CompressedCalendarState(const RawCalendarState value = kInvalidCalendarState) :
    value_(value) {}
  ~CompressedCalendarState() = default;

  constexpr auto Get() const -> RawCalendarState {
    return value_;
  }
};
}  // namespace prt

#endif  // PRT_CALENDAR_STATE_COMPRESSED_H
