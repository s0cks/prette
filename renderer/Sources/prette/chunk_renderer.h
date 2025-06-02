#ifndef PRT_CHUNK_RENDERER_H
#define PRT_CHUNK_RENDERER_H

#include <cstdint>
#include <vector>

#include "prette/chunk/chunk.h"
#include "prette/chunk_mesh.h"
#include "prette/common.h"
#include "prette/descriptor_set.h"
#include "prette/descriptor_set_layout.h"
#include "prette/texture.h"
#include "prette/tile.h"
#include "prette/tile_mesh.h"
#include "prette/vk.h"

namespace prt {
class ChunkRenderer {
 private:
  vk::RenderPipeline* tile_pipeline_ = nullptr;
  TileMesh* tile_mesh_ = nullptr;
  std::vector<vk::Buffer*> material_buffers_{};
  vk::DescriptorSetLayout* tile_descriptors_layout_ = nullptr;
  std::vector<vk::DescriptorSet*> tile_descriptors_{};
  Texture* texture_ = nullptr;

  void UpdateChunkBuffer(Chunk* chunk, const bool staging = true);
  void RenderChunkMesh(VkCommandBuffer buffer, ChunkMesh* chunk, const uint64_t num_instances = 1);

 public:
  ChunkRenderer();
  ~ChunkRenderer();

  auto GetTileDescriptorSetLayout() const -> vk::DescriptorSetLayout* {
    return tile_descriptors_layout_;
  }

  auto GetTileMesh() const -> TileMesh* {
    return tile_mesh_;
  }

  auto IsInitialized() const -> bool {
    return true;
  }

  void Render(VkCommandBuffer buffer, Chunk* chunk);
};
}  // namespace prt

#endif  // PRT_CHUNK_RENDERER_H
