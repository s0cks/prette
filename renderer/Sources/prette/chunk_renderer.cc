#include "prette/chunk_renderer.h"

#include "prette/camera.h"
#include "prette/chunk/chunk.h"
#include "prette/chunk/chunk_metadata.h"
#include "prette/chunk_mesh.h"
#include "prette/descriptor_set.h"
#include "prette/descriptor_set_builder.h"
#include "prette/descriptor_set_update.h"
#include "prette/pipeline/pipeline.h"
#include "prette/render_pass/scene_renderer.h"
#include "prette/renderer.h"
#include "prette/renderer_event.h"
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

ChunkRenderer::ChunkRenderer() {
  OnInitDescriptorSets([this](InitDescriptorSetsEvent* event) {
    descriptors_ = CreateDescriptors();
    ASSERT_INITIALIZED(descriptors_);
  });
  OnInitGraphicsPipelines([this](InitGraphicsPipelinesEvent* event) {
    pipeline_ = vk::RenderPipeline::FromJson("chunk.json");
    ASSERT_INITIALIZED(pipeline_);
  });
  OnInitBuffers([this](InitBuffersEvent* event) {
    chunk_ = CreateChunkBuffer();
    mesh_ = NewChunkMesh();
    ASSERT_INITIALIZED(mesh_);
    UpdateDescriptorSet();
  });
}

ChunkRenderer::~ChunkRenderer() {
  delete chunk_;
  delete mesh_;
}

void ChunkRenderer::UpdateDescriptorSet() {
  vk::DescriptorSetUpdate update(GetDescriptors());
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
  GetPipeline()->Bind(&buffer);
  vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *GetPipeline()->GetPipelineLayout(), 0, 1,
                          &GetDescriptors()->GetHandle(), 0, nullptr);
  GetMesh()->Draw(buffer);
  chunk->UnmarkDirty();
}
}  // namespace prt