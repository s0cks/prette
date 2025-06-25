#include "prette/city/housing.h"
#include "prette/to_string.h"

namespace prt {
auto Housing::ToString() const -> std::string {
  ToStringHelper<Housing> helper{};
  helper.AddFieldRef("available_units", available_units);
  return helper;
}
}  // namespace prt