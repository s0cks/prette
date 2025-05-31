#include "prette/chunk_mesh.h"

#include <array>

#include "prette/atlas1.h"
#include "prette/chunk/chunk.h"
#include "prette/chunk/chunk_metadata.h"
#include "prette/common.h"
#include "prette/copy_to_buffer.h"
#include "prette/index_buffer.h"
#include "prette/tile.h"
#include "prette/vertex/vertex2d.h"
#include "prette/vertex/vertex_buffer.h"
#include "prette/vk_buffer.h"

namespace prt {
static const ChunkMeshClass::VertexArray kTileVertices = {
    tex2d::Vertex{
        // top left
        .pos = {-0.5f, -0.5f},
        .uv = atlas1::container::kMinUV,
    },
    {
        // top right
        .pos = {0.5f, -0.5f},
        .uv = {atlas1::container::kMaxUV.x, atlas1::container::kMinUV.y},
    },
    {
        // bottom right
        .pos = {0.5f, 0.5f},
        .uv = atlas1::container::kMaxUV,
    },
    {
        // bottom left
        .pos = {-0.5f, 0.5f},
        .uv = {atlas1::container::kMinUV.x, atlas1::container::kMaxUV.y},
    },
};
static const ChunkMeshClass::IndexArray kTileIndices = {
    0, 1, 2, 2, 3, 0,
};

static inline auto CreateChunkVertexBuffer() -> vk::Buffer* {
  vk::VertexBufferBuilder<ChunkMeshClass::VertexType> builder{};
  // clang-format off
  const auto buffer = builder.WithLength(ChunkMeshClass::kTotalNumberOfVertices)
    .WithTransferDestUsage()
    .Build();
  // clang-format on
  ASSERT(buffer);
  vk::CopyBytesToBufferWithStaging::Copy(&kTileVertices[0], kTileVertices.size(), buffer);
  return buffer;
}

static inline auto CreateChunkIndexBuffer() -> vk::Buffer* {
  vk::IndexBufferBuilder<ChunkMeshClass::IndexType> builder{};
  // clang-format off
  const auto buffer = builder.WithLength(ChunkMeshClass::kTotalNumberOfIndices)
    .WithTransferDestUsage()
    .Build();
  // clang-format on
  ASSERT(buffer);
  vk::CopyBytesToBufferWithStaging::Copy(&kTileIndices[0], kTileIndices.size(), buffer);
  return buffer;
}

auto NewChunkMesh() -> ChunkMesh* {
  return new ChunkMesh(CreateChunkVertexBuffer(), CreateChunkIndexBuffer());
}

class ChunkMeshifier : public TileVisitor {
  using InstanceData = std::array<TileData, kTotalChunkSize>;

 private:
  ChunkMesh* mesh_;
  InstanceData data_{};
  uint64_t current_ = 0;

 public:
  explicit ChunkMeshifier(ChunkMesh* mesh) :
    TileVisitor(),
    mesh_(mesh) {
    ASSERT(mesh_);
  }
  ~ChunkMeshifier() override = default;

  auto GetMesh() const -> ChunkMesh* {
    return mesh_;
  }

  auto data() const -> const InstanceData& {
    return data_;
  }

  auto Visit(Tile* tile) -> bool override {
    ASSERT(tile);
    auto& data = data_.at(current_++);
    data = tile->data();
    return true;
  }

  auto Meshify(Chunk* chunk) -> bool {
    ASSERT(chunk);
    if (!chunk->VisitTiles(this))
      return false;
    vk::CopyBytesToBufferWithStaging::Copy(data_, GetMesh()->GetTileBuffer());
    return true;
  }

  auto operator()(Chunk* chunk) -> bool {
    return Meshify(chunk);
  }
};

auto MeshifyChunk(Chunk* chunk, ChunkMesh* mesh) -> bool {
  ASSERT(chunk);
  ASSERT_INITIALIZED(mesh);
  ChunkMeshifier meshifier(mesh);
  LOG_IF(FATAL, !meshifier(chunk)) << "failed to instance chunk tiles.";
  return mesh;
}
}  // namespace prt