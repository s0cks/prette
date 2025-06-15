#include "prette/road.h"

#include "prette/to_string.h"

namespace prt {
auto Road::ToString() const -> std::string {
  ToStringHelper<Road> helper{};
  helper.AddField("name", GetName());
  return helper;
}
}  // namespace prt