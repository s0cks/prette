#include "prette/city/electricity.h"

#include "prette/to_string.h"

namespace prt::electricity {
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
}  // namespace prt::electricity