#ifndef PRT_CHUNK_RENDERER_H
#define PRT_CHUNK_RENDERER_H

#include "prette/chunk.h"
#include "prette/chunk_mesh.h"
#include "prette/descriptor_set.h"
#include "prette/pipeline.h"
#include "prette/tile.h"
#include "prette/vk.h"

namespace prt {
class ChunkRenderer {
 private:
  vk::RenderPipeline* pipeline_;
  ChunkMetadataBuffer* chunk_;
  ChunkMesh* mesh_;
  vk::DescriptorSet* descriptors_;

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

  void Render(VkCommandBuffer buffer, Chunk* chunk);
};
}  // namespace prt

#endif  // PRT_CHUNK_RENDERER_H
