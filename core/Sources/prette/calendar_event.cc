#include "prette/calendar_event.h"

#include <cstdint>

#include "prette/to_string.h"

namespace prt {
auto NewDayEvent::ToString() const -> std::string {
  ToStringHelper<NewDayEvent> helper{};
  helper.AddFieldRef("day", static_cast<uint64_t>(GetDay()));
  return helper;
}

auto NewMonthEvent::ToString() const -> std::string {
  ToStringHelper<NewMonthEvent> helper{};
  helper.AddFieldRef("month", static_cast<uint64_t>(GetMonth()));
  return helper;
}

auto NewYearEvent::ToString() const -> std::string {
  ToStringHelper<NewYearEvent> helper{};
  helper.AddFieldRef("year", static_cast<uint64_t>(GetYear()));
  return helper;
}

auto NewSeasonEvent::ToString() const -> std::string {
  ToStringHelper<NewSeasonEvent> helper{};
  helper.AddFieldRef("season", GetSeason());
  return helper;
}
}  // namespace prt