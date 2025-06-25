#include "prette/city/water.h"

#include <string>

#include "prette/to_string.h"

namespace prt::water {
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
}  // namespace prt::water