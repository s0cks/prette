#ifndef PRT_CHUNK_MESH_H
#define PRT_CHUNK_MESH_H

#include "prette/common.h"
#include "prette/mesh_class.h"
#include "prette/platform.h"
#include "prette/tile.h"
#include "prette/vertex/vertex2d.h"

namespace prt {
struct ChunkMeshClass : public vk::MeshClass<TileData, color2d::Vertex, 4, uint16_t, 6> {
  static constexpr const auto kClassName = "chunk";
};

}  // namespace prt

#endif  // PRT_CHUNK_MESH_H
