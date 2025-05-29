#include "prette/chunk_renderer.h"

#include <cstdint>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/camera.h"
#include "prette/chunk/chunk.h"
#include "prette/chunk/chunk_metadata.h"
#include "prette/chunk_mesh.h"
#include "prette/common.h"
#include "prette/copy_to_buffer.h"
#include "prette/descriptor_set.h"
#include "prette/descriptor_set_builder.h"
#include "prette/descriptor_set_update.h"
#include "prette/flags.h"
#include "prette/index_buffer.h"
#include "prette/pipeline/pipeline.h"
#include "prette/renderer.h"
#include "prette/renderer_event.h"
#include "prette/texture.h"
#include "prette/tile.h"
#include "prette/vertex/vertex_buffer.h"
#include "prette/vk.h"

namespace prt {
static inline auto CreateDescriptors() -> vk::DescriptorSet* {
  vk::DescriptorSetBuilder builder;
  builder.WithName("chunk");
  builder.AddUniformBufferBinding();
  builder.AddUniformBufferBinding();
  builder.AddUniformBufferBinding();
  builder.AddCombinedImageSamplerBinding(VK_SHADER_STAGE_FRAGMENT_BIT);
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
    TextureBuilder builder("wood");
    texture_ = builder.WithTextureData(ReadTexture(fs::path(FLAGS_resources) / "textures" / "wood.png", true),
                                       VK_FORMAT_R8G8B8A8_SRGB);
    ASSERT_INITIALIZED(texture_);
    mesh_ = NewChunkMesh();
    ASSERT_INITIALIZED(mesh_);
    UpdateDescriptorSet();
  });
}

ChunkRenderer::~ChunkRenderer() {
  delete mesh_;
  delete descriptors_;
}

void ChunkRenderer::UpdateDescriptorSet() {
  vk::DescriptorSetUpdate update(GetDescriptors());
  update.AddWriteUniformBuffer(0).WithBufferInfo(GetCamera()->GetBuffer());
  update.AddWriteUniformBuffer(1).WithBufferInfo(GetMesh()->GetMetadataBuffer());
  update.AddWriteUniformBuffer(2).WithBufferInfo(GetMesh()->GetTileBuffer());
  update.AddWriteCombinedImageSampler(3).WithImageInfo(&texture_->GetDescriptor());
}

void ChunkRenderer::UpdateChunkBuffer(Chunk* chunk, const bool staging) {
  ASSERT(chunk);
  ChunkMetadata data{};
  data.id = 0;  // TODO: fill in
  data.pos = chunk->GetPos();
  if (staging) {
    vk::CopyBytesToBufferWithStaging copy(data);
    copy(GetMesh()->GetMetadataBuffer());
  } else {
    vk::CopyBytesToBuffer copy(data);
    copy(GetMesh()->GetMetadataBuffer());
  }
}

void ChunkRenderer::RenderChunkMesh(VkCommandBuffer buffer, ChunkMesh* mesh, const uint64_t num_instances) {
  ASSERT_INITIALIZED(mesh);
  ASSERT(num_instances >= 1);
  vk::BindVertexBuffer(buffer, mesh->GetVertexBuffer());
  vk::BindIndexBuffer<uint16_t>(buffer, mesh->GetIndexBuffer());
  vkCmdDrawIndexed(buffer, ChunkMeshClass::kTotalNumberOfIndices, num_instances, 0, 0, 0);
}

void ChunkRenderer::Render(VkCommandBuffer buffer, Chunk* chunk) {
  UpdateChunkBuffer(chunk);
  if (chunk->IsDirty())
    MeshifyChunk(chunk, GetMesh());
  GetPipeline()->Bind(&buffer);
  vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *GetPipeline()->GetPipelineLayout(), 0, 1,
                          &GetDescriptors()->GetHandle(), 0, nullptr);
  RenderChunkMesh(buffer, GetMesh());
  chunk->UnmarkDirty();
}
}  // namespace prt