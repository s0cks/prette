#ifndef PRT_TILE_H
#define PRT_TILE_H

#include "prette/chunk_generated.h"
#include "prette/common.h"
#include "prette/glm.h"
#include "prette/material.h"

namespace prt {
using TilePos = glm::u32vec2;

class Tile;
class TileVisitor {
 protected:
  TileVisitor() = default;

 public:
  virtual ~TileVisitor() = default;
  virtual auto Visit(Tile* tile) -> bool = 0;
};

static constexpr const auto kDefaultTileMaterial = 1;

class Tile {
  friend class Chunk;
  DEFINE_DEFAULT_COPYABLE_TYPE(Tile);

 private:
  TilePos pos_{};
  MaterialId material_;

 public:
  Tile() = default;
  Tile(const TilePos pos, const raw::Tile raw) :
    pos_(std::move(pos)),
    material_(raw.material()) {}
  Tile(const TilePos pos, const MaterialId material = kDefaultTileMaterial) :
    pos_(std::move(pos)),
    material_(material) {}
  Tile(const uint32_t x, const uint32_t y, const MaterialId material) :
    Tile(TilePos(x, y), material) {}
  ~Tile() = default;

  auto GetPos() const -> const TilePos& {
    return pos_;
  }

  auto GetMaterialId() const -> MaterialId {
    return material_;
  }

  void SetMaterialId(const MaterialId rhs) {
    material_ = rhs;
  }

  auto Accept(TileVisitor* vis) -> bool {
    ASSERT(vis);
    return vis->Visit(this);
  }

  auto ToString() const -> std::string;

  auto operator=(const raw::Tile& rhs) -> Tile& {
    material_ = rhs.material();
    return *this;
  }

  friend auto operator<<(std::ostream& stream, const Tile& rhs) -> std::ostream& {
    return stream << rhs.ToString();
  }
};
}  // namespace prt

#endif  // PRT_TILE_H
