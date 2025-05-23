#include "prette/chunk_renderer.h"

#include "prette/camera.h"
#include "prette/chunk.h"
#include "prette/chunk_mesh.h"
#include "prette/common.h"
#include "prette/descriptor_set.h"
#include "prette/descriptor_set_builder.h"
#include "prette/descriptor_set_update.h"
#include "prette/pipeline.h"
#include "prette/scene_renderer.h"
#include "prette/tile.h"
#include "prette/vk.h"

namespace prt {
static inline auto CreateChunkBuffer() -> ChunkMetadataBuffer* {
  return new ChunkMetadataBuffer();
}

static inline auto CreateDescriptors() -> vk::DescriptorSet* {
  vk::DescriptorSetBuilder builder;
  builder.WithName("chunk");
  builder.AddUniformBufferBinding();
  builder.AddUniformBufferBinding();
  builder.AddUniformBufferBinding();
  return builder.Build();
}

ChunkRenderer::ChunkRenderer() :
  chunk_(CreateChunkBuffer()),
  mesh_(NewChunkMesh()),
  descriptors_(CreateDescriptors()) {
  ASSERT_INITIALIZED(chunk_);
  ASSERT_INITIALIZED(mesh_);
  ASSERT_INITIALIZED(descriptors_);
  UpdateDescriptorSet();
  pipeline_ = vk::RenderPipeline::FromJson("chunk.json");
  ASSERT_INITIALIZED(pipeline_);
}

ChunkRenderer::~ChunkRenderer() {
  delete chunk_;
  delete mesh_;
}

void ChunkRenderer::UpdateDescriptorSet() {
  vk::DescriptorSetUpdate update(*GetDescriptors());
  update.AddWriteUniformBuffer(0).WithBufferInfo(GetCamera()->GetBuffer());
  update.AddWriteUniformBuffer(1).WithBufferInfo(GetChunkBuffer());
  update.AddWriteUniformBuffer(2).WithBufferInfo(GetMesh()->GetInstanceBuffer());
}

void ChunkRenderer::UpdateChunkBuffer(Chunk* chunk, const bool staging) {
  ASSERT(chunk);
  ChunkMetadata data{};
  data.id = 0;  // TODO: fill in
  data.pos = chunk->GetPos();
  GetChunkBuffer()->SyncData(data, staging);
}

void ChunkRenderer::Render(VkCommandBuffer buffer, Chunk* chunk) {
  UpdateChunkBuffer(chunk);
  if (chunk->IsDirty())
    MeshifyChunk(chunk, GetMesh());
  if (GetMesh()->IsDirty())
    GetMesh()->Sync(true);
  vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *GetPipeline()->GetPipelineLayout(), 0, 1,
                          &GetDescriptors()->GetHandle(), 0, nullptr);
  GetPipeline()->Bind(&buffer);
  GetMesh()->Draw(buffer);
  chunk->UnmarkDirty();
}
}  // namespace prt