#ifndef PRT_CHUNK_RENDERER_H
#define PRT_CHUNK_RENDERER_H

#include "prette/chunk/chunk.h"
#include "prette/chunk/chunk_metadata.h"
#include "prette/chunk_mesh.h"
#include "prette/descriptor_set.h"
#include "prette/pipeline/pipeline.h"
#include "prette/tile.h"
#include "prette/uniform_buffer.h"
#include "prette/vk.h"

namespace prt {
using ChunkMetadataBuffer = vk::UniformBuffer<ChunkMetadata, 1>;

class ChunkRenderer {
 private:
  vk::RenderPipeline* pipeline_ = nullptr;
  ChunkMetadataBuffer* chunk_ = nullptr;
  ChunkMesh* mesh_ = nullptr;
  vk::DescriptorSet* descriptors_ = nullptr;

  void UpdateChunkBuffer(Chunk* chunk, const bool staging = true);
  void UpdateDescriptorSet();

 public:
  ChunkRenderer();
  ~ChunkRenderer();

  auto GetMesh() const -> ChunkMesh* {
    return mesh_;
  }

  auto GetChunkBuffer() const -> ChunkMetadataBuffer* {
    return chunk_;
  }

  auto GetPipeline() const -> vk::RenderPipeline* {
    return pipeline_;
  }

  auto GetDescriptors() const -> vk::DescriptorSet* {
    return descriptors_;
  }

  auto IsInitialized() const -> bool {
    return vk::AllInitialized(pipeline_, chunk_, descriptors_) && mesh_;
  }

  void Render(VkCommandBuffer buffer, Chunk* chunk);
};
}  // namespace prt

#endif  // PRT_CHUNK_RENDERER_H
