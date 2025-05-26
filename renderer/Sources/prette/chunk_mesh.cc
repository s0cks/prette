#include "prette/chunk_mesh.h"

#include <array>

#include "prette/chunk/chunk.h"
#include "prette/common.h"
#include "prette/tile.h"
#include "prette/vertex/vertex2d.h"

namespace prt {
static const ChunkMesh::VertexArray kTileVertices = {
    color2d::Vertex{
        .pos = {-0.5f, -0.5f},
        .color = {0.0f, 0.0f, 0.0f, 1.0f},
    },
    {
        .pos = {0.5f, -0.5f},
        .color = {0.0f, 0.0f, 0.0f, 1.0f},
    },
    {
        .pos = {0.5f, 0.5f},
        .color = {0.0f, 0.0f, 0.0f, 1.0f},
    },
    {
        .pos = {-0.5f, 0.5f},
        .color = {0.0f, 0.0f, 0.0f, 1.0f},
    },
};
static const ChunkMesh::IndexArray kTileIndices = {
    0, 1, 2, 2, 3, 0,
};

auto NewChunkMesh(const uint64_t num_tiles) -> ChunkMesh* {
  return new ChunkMesh(num_tiles, kTileVertices, kTileIndices);
}

class ChunkMeshifier : public TileVisitor {
  using InstanceData = std::array<TileData, kTotalChunkSize>;

 private:
  InstanceData data_{};
  uint64_t current_ = 0;

 public:
  ChunkMeshifier() = default;
  ~ChunkMeshifier() override = default;

  auto data() const -> const InstanceData& {
    return data_;
  }

  auto Visit(Tile* tile) -> bool override {
    ASSERT(tile);
    auto& data = data_.at(current_++);
    data = tile->data();
    return true;
  }

  auto operator()(Chunk* chunk) -> bool {
    ASSERT(chunk);
    return chunk->VisitTiles(this);
  }
};

auto MeshifyChunk(Chunk* chunk, ChunkMesh* mesh) -> ChunkMesh* {
  ASSERT(chunk);
  ASSERT_INITIALIZED(mesh);
  ChunkMeshifier meshifier{};
  LOG_IF(FATAL, !meshifier(chunk)) << "failed to instance chunk tiles.";
  mesh->SetInstances(meshifier.data());
  return mesh;
}
}  // namespace prt