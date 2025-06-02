#include "prette/tile.h"

#include <cstdint>
#include <string>
#include <utility>

#include "prette/chunk/chunk.h"
#include "prette/glm.h"
#include "prette/to_string.h"

namespace prt {
Tile::Tile(Chunk* owner, const TilePos pos, const raw::Tile raw) :
  owner_(owner),
  data_() {
  SetPos(std::move(pos));
  SetMaterial(raw.material());
}
Tile::Tile(Chunk* owner, const TilePos pos, const uint64_t material) :
  owner_(owner),
  data_() {
  SetPos(pos);
  SetMaterial(material);
}

auto Tile::ToString() const -> std::string {
  ToStringHelper<Tile> helper{};
  helper.AddFieldRef("pos", glm::to_string(GetPos()));
  helper.AddFieldRef("material", GetMaterial());
  return helper;
}

auto Tile::Contains(const glm::vec2& pos) const -> bool {
  return pos.x >= (GetPos().x - 0.5f) && pos.x <= (GetPos().x + 0.5f) && pos.y >= (GetPos().y - 0.5f) &&
         pos.y <= (GetPos().y + 0.5f);
}

void Tile::SetHovering(const bool rhs) {
  data_.hovering = rhs;
  GetOwner()->MarkDirty();
}

void Tile::SetPos(const TilePos rhs) {
  data_.pos = std::move(rhs);
  GetOwner()->MarkDirty();
}

void Tile::SetMaterial(const uint64_t rhs) {
  data_.material = rhs;
  GetOwner()->MarkDirty();
}
}  // namespace prt