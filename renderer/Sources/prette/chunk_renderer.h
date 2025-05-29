#ifndef PRT_CHUNK_RENDERER_H
#define PRT_CHUNK_RENDERER_H

#include <cstdint>

#include "prette/chunk/chunk.h"
#include "prette/chunk_mesh.h"
#include "prette/descriptor_set.h"
#include "prette/pipeline/pipeline.h"
#include "prette/texture.h"
#include "prette/tile.h"
#include "prette/vk.h"

namespace prt {
class ChunkRenderer {
 private:
  vk::RenderPipeline* pipeline_ = nullptr;
  ChunkMesh* mesh_ = nullptr;
  vk::DescriptorSet* descriptors_ = nullptr;
  Texture* texture_ = nullptr;

  void UpdateChunkBuffer(Chunk* chunk, const bool staging = true);
  void UpdateDescriptorSet();
  void RenderChunkMesh(VkCommandBuffer buffer, ChunkMesh* chunk, const uint64_t num_instances = 1);

 public:
  ChunkRenderer();
  ~ChunkRenderer();

  auto GetMesh() const -> ChunkMesh* {
    return mesh_;
  }

  auto GetPipeline() const -> vk::RenderPipeline* {
    return pipeline_;
  }

  auto GetDescriptors() const -> vk::DescriptorSet* {
    return descriptors_;
  }

  auto IsInitialized() const -> bool {
    return vk::AllInitialized(pipeline_, descriptors_) && mesh_;
  }

  void Render(VkCommandBuffer buffer, Chunk* chunk);
};
}  // namespace prt

#endif  // PRT_CHUNK_RENDERER_H
