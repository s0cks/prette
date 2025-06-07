#ifndef PRT_CHUNK_RENDERER_H
#define PRT_CHUNK_RENDERER_H

#include <cstdint>
#include <vector>

#include "prette/chunk_mesh.h"
#include "prette/common.h"
#include "prette/descriptor_set.h"
#include "prette/descriptor_set_layout.h"
#include "prette/texture.h"
#include "prette/tile_mesh.h"
#include "prette/vk.h"

namespace prt {
static constexpr const auto kMaxNumberOfChunks = 32;
class ChunkRenderer {
 private:
  vk::RenderPipeline* tile_pipeline_ = nullptr;
  TileMesh* tile_mesh_ = nullptr;
  std::vector<vk::Buffer*> material_buffers_{};
  vk::DescriptorSetLayout* tile_descriptors_layout_ = nullptr;
  std::vector<vk::DescriptorSet*> tile_descriptors_{};
  vk::DescriptorSet* chunk_descriptors_ = nullptr;
  vk::Buffer* chunk_data_ = nullptr;
  Texture* texture_ = nullptr;

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

  void RenderChunks(VkCommandBuffer buffer);
};
}  // namespace prt

#endif  // PRT_CHUNK_RENDERER_H
