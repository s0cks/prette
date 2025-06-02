#ifndef PRT_TILE_MESH_H
#define PRT_TILE_MESH_H

#include <cstdint>

#include "prette/mesh/mesh.h"
#include "prette/mesh/mesh_class.h"
#include "prette/tile.h"
#include "prette/vertex/vertex2d.h"

namespace prt {
struct TileMeshClass : public vk::MeshClass<TileData, bare2d::Vertex, 4, uint16_t, 6> {
  static constexpr const auto kClassName = "tile";
};

using TileMesh = vk::IndexedMesh;

auto NewTileMesh() -> TileMesh*;
auto GetTileNormal() -> glm::vec2;
}  // namespace prt

#endif  // PRT_TILE_MESH_H
