#include "prette/tile.h"

#include "prette/to_string.h"

namespace prt {
auto Tile::ToString() const -> std::string {
  ToStringHelper<Tile> helper{};
  helper.AddFieldRef("pos", glm::to_string(GetPos()));
  helper.AddFieldRef("material", GetMaterialId());
  return helper;
}
}  // namespace prt