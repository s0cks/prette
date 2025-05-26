#ifndef PRT_CHUNK_MESH_H
#define PRT_CHUNK_MESH_H

#include "prette/chunk/chunk.h"
#include "prette/chunk/chunk_metadata.h"
#include "prette/common.h"
#include "prette/mesh.h"
#include "prette/mesh_class.h"
#include "prette/platform.h"
#include "prette/tile.h"
#include "prette/vertex/vertex2d.h"

namespace prt {
struct ChunkMeshClass : public vk::MeshClass<TileData, color2d::Vertex, 4, uint16_t, 6> {
  static constexpr const auto kClassName = "chunk";
};

using ChunkMesh = vk::InstancedMesh<ChunkMeshClass, kTotalChunkSize>;

auto NewChunkMesh(const uint64_t num_tiles = kTotalChunkSize) -> ChunkMesh*;
auto MeshifyChunk(Chunk* chunk, ChunkMesh* mesh) -> ChunkMesh*;

static inline auto MeshifyNewChunk(Chunk* chunk) -> ChunkMesh* {
  ASSERT(chunk);
  return MeshifyChunk(chunk, NewChunkMesh());
}
}  // namespace prt

#endif  // PRT_CHUNK_MESH_H
