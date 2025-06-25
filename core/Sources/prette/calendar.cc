#include "prette/calendar.h"

#include <array>

#include "prette/calendar_state.h"
#include "prette/season.h"
#include "prette/tick.h"

namespace prt {
static constexpr const std::array<CalendarStatePattern, kTotalNumberOfSeasons> kSeasonTable = {
    // Spring
    CalendarStatePattern{
        .day = 20,
        .month = 2,
    },
    // summer
    CalendarStatePattern{
        .day = 21,
        .month = 5,
    },
    // fall
    CalendarStatePattern{
        .day = 22,
        .month = 8,
    },
    // winter
    CalendarStatePattern{
        .day = 21,
        .month = 11,
    },
};

static inline constexpr auto IsNewSeason(const CalendarState& state) -> bool {
  for (auto idx = 0; idx < kTotalNumberOfSeasons; idx++) {
    if (kSeasonTable.at(idx) == state)
      return true;
  }
  return false;
}

void Calendar::Update(const TickDelta delta) {
  state_.hour += 1;
  if (state_.IsNewDay()) {
    NewDay();
    if (IsNewSeason(state_))
      NewSeason();
    if (state_.IsNewMonth()) {
      NewMonth();
      if (state_.IsNewYear())
        NewYear();
    }
  }
}
}  // namespace prt