#include "prette/city/building.h"

#include <string>

#include "prette/glm.h"
#include "prette/to_string.h"

namespace prt {
auto Building::ToString() const -> std::string {
  ToStringHelper<Building> helper{};
  helper.AddField("pos", glm::to_string(pos));
  return helper;
}
}  // namespace prt