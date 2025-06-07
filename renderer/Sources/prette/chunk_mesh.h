#ifndef PRT_CHUNK_MESH_H
#define PRT_CHUNK_MESH_H

#include <concepts>
#include <cstdint>

#include "prette/assertions.h"
#include "prette/chunk/chunk_metadata.h"
#include "prette/mesh/mesh.h"
#include "prette/mesh/mesh_class.h"
#include "prette/tile.h"
#include "prette/uniform_buffer.h"
#include "prette/vertex/vertex2d.h"
#include "prette/vk.h"
#include "prette/vk_buffer.h"

namespace prt {
struct ChunkMeshClass : public vk::MeshClass<TileData, tex2d::Vertex, 4, uint16_t, 6> {
  static constexpr const auto kClassName = "chunk";
};

class ChunkMesh : public vk::IndexedMesh {
  using MeshClass = ChunkMeshClass;

 private:
  vk::Buffer* meta_;
  vk::Buffer* tiles_;

 public:
  ChunkMesh(vk::Buffer* vertices, vk::Buffer* indices) :
    vk::IndexedMesh(vertices, indices) {
    {
      vk::UniformBufferBuilder<ChunkData> builder(1);
      meta_ = builder.WithTransferDestUsage();
      ASSERT_INITIALIZED(meta_);
    }
    {
      vk::UniformBufferBuilder<TileData> builder(kTotalNumberOfTilesPerChunk);
      tiles_ = builder.WithTransferDestUsage();
      ASSERT_INITIALIZED(tiles_);
    }
  }
  ~ChunkMesh() override = default;

  auto GetMetadataBuffer() const -> vk::Buffer* {
    return meta_;
  }

  auto GetTileBuffer() const -> vk::Buffer* {
    return tiles_;
  }
};

auto NewChunkMesh() -> ChunkMesh*;

template <typename T>
concept IsChunkMesh = requires {
  { T::MeshClass } -> std::convertible_to<ChunkMeshClass>;
};

auto MeshifyChunk(Chunk* chunk, ChunkMesh* mesh) -> bool;
}  // namespace prt

#endif  // PRT_CHUNK_MESH_H
