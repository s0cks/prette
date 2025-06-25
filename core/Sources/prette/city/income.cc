#include "prette/city/income.h"

#include "prette/to_string.h"

namespace prt::income {
auto Sink::ToString() const -> std::string {
  ToStringHelper<Sink> helper{};
  helper.AddFieldRef("per_tick", per_tick);
  return helper;
}

auto Source::ToString() const -> std::string {
  ToStringHelper<Source> helper{};
  helper.AddFieldRef("per_tick", per_tick);
  return helper;
}
}  // namespace prt::income