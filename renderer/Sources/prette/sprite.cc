#include "prette/sprite.h"

#include <cstdint>
#include <filesystem>
#include <utility>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/atlas1.h"
#include "prette/camera.h"
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
#include "prette/uniform_buffer.h"
#include "prette/vertex/vertex2d.h"
#include "prette/vertex/vertex_buffer.h"
#include "prette/vk_buffer.h"

namespace prt {
static inline auto CreateDescriptors() -> vk::DescriptorSet* {
  vk::DescriptorSetBuilder builder{};
  builder.WithName("sprite");
  builder.AddUniformBufferBinding();                                                  // camera
  builder.AddBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);  // instances
  builder.AddCombinedImageSamplerBinding(VK_SHADER_STAGE_FRAGMENT_BIT);               // atlas
  return builder.Build();
}

static inline auto NewVertexBuffer(const uint64_t init_cap) -> vk::Buffer* {
  const auto total_length = init_cap * SpriteMeshClass::kTotalNumberOfVertices;
  SpriteMeshClass::VertexBufferBuilder builder(total_length);
  vk::Buffer* buffer = builder.WithTransferDestUsage();
  return buffer;
}

static inline auto NewIndexBuffer(const uint64_t init_cap) -> vk::Buffer* {
  const auto total_length = init_cap * SpriteMeshClass::kTotalNumberOfIndices;
  SpriteMeshClass::IndexBufferBuilder builder(total_length);
  return builder.WithTransferDestUsage();
}

static inline auto NewSpritesBuffer(const uint64_t init_cap) -> vk::Buffer* {
  const auto total_length = init_cap;
  vk::UniformBufferBuilder<SpriteData> builder(total_length);
  return builder.WithTransferDestUsage().WithStorageUsage();
}

SpriteRenderer::SpriteRenderer() {
  OnInitDescriptorSets([this](InitDescriptorSetsEvent* event) {
    descriptors_ = CreateDescriptors();
    ASSERT_INITIALIZED(descriptors_);
  });
  OnInitGraphicsPipelines([this](InitGraphicsPipelinesEvent* event) {
    pipeline_ = vk::RenderPipeline::FromJson("sprite.json");
    ASSERT_INITIALIZED(pipeline_);
  });
  OnInitBuffers([this](InitBuffersEvent* event) {
    TextureBuilder builder("atlas1");
    spritesheet_ = builder.WithTextureData(
        ReadTexture(fs::path(FLAGS_resources) / "textures" / "atlas1" / "atlas1.png", true), VK_FORMAT_R8G8B8A8_SRGB);
    vertices_.buffer = NewVertexBuffer(kInitCapacity);
    ASSERT_INITIALIZED(vertices_.buffer);
    vertices_.data.reserve(kInitCapacity * SpriteMeshClass::kTotalNumberOfVertices);

    indices_.buffer = NewIndexBuffer(kInitCapacity);
    ASSERT_INITIALIZED(indices_.buffer);
    indices_.data.reserve(kInitCapacity * SpriteMeshClass::kTotalNumberOfIndices);

    sprites_.buffer = NewSpritesBuffer(kInitCapacity);
    ASSERT_INITIALIZED(sprites_.buffer);
    sprites_.data.reserve(kInitCapacity);
    UpdateDescriptors();
  });
}

SpriteRenderer::~SpriteRenderer() {
  delete descriptors_;
  delete vertices_.buffer;
  delete indices_.buffer;
}

void SpriteRenderer::AppendSpriteIndices(const uint64_t start) {
  const auto idx_start = indices_.data.size();
  indices_.data.resize(idx_start + SpriteMeshClass::kTotalNumberOfIndices);
  indices_.data[idx_start + 0] = start + 0;
  indices_.data[idx_start + 1] = start + 1;
  indices_.data[idx_start + 2] = start + 2;
  indices_.data[idx_start + 3] = start + 2;
  indices_.data[idx_start + 4] = start + 3;
  indices_.data[idx_start + 5] = start + 0;
  return;
}

auto SpriteRenderer::AppendSpriteVertices(const glm::vec2& min_uv, const glm::vec2& max_uv)
    -> std::pair<uint64_t, uint64_t> {
  const auto start_idx = vertices_.data.size();
  vertices_.data.resize(start_idx + SpriteMeshClass::kTotalNumberOfVertices);
  vertices_.data[start_idx + 0] = tex2d::Vertex{
      // top left
      .pos = {-0.5f, -0.5f},
      .uv = min_uv,
  };
  vertices_.data[start_idx + 1] = {
      // top right
      .pos = {0.5f, -0.5f},
      .uv = {max_uv.x, min_uv.y},
  };
  vertices_.data[start_idx + 2] = {
      // bottom right
      .pos = {0.5f, 0.5f},
      .uv = max_uv,
  };
  vertices_.data[start_idx + 3] = {
      // bottom left
      .pos = {-0.5f, 0.5f},
      .uv = {min_uv.x, max_uv.y},
  };
  return {start_idx, 4};
}

void SpriteRenderer::AddSprite(const glm::vec2 pos, const glm::vec2 min_uv, const glm::vec2 max_uv) {
  const auto [start_vertex, num_vertices] = AppendSpriteVertices(min_uv, max_uv);
  AppendSpriteIndices(start_vertex);
  sprites_.data.push_back(SpriteData{
      .model = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 1.0f)),
      .pos = pos,
  });
  sprites_changed_ = true;
  return;
}

void SpriteRenderer::UpdateDescriptors() {
  vk::DescriptorSetUpdate update(GetDescriptors());
  update.AddWriteCameraUniformBuffer(0, GetCamera());
  update.AddWriteStorageBuffer(1).WithBufferInfo(GetSpritesBuffer());
  update.AddWriteCombinedImageSampler(2).WithImageInfo(spritesheet_);
}

void SpriteRenderer::UpdateVertexBuffer() {
  vk::CopyBytesToBufferWithStaging::Copy<tex2d::Vertex>(&vertices_.data[0], vertices_.data.size(), vertices_.buffer);
}

void SpriteRenderer::UpdateIndexBuffer() {
  vk::CopyBytesToBufferWithStaging::Copy<uint16_t>(&indices_.data[0], indices_.data.size(), indices_.buffer);
}

void SpriteRenderer::UpdateSpritesBuffer() {
  vk::CopyBytesToBufferWithStaging::Copy<SpriteData>(&sprites_.data[0], sprites_.data.size(), sprites_.buffer);
}

void SpriteRenderer::Render(VkCommandBuffer cmd) {
  if (first_) {
    AddSprite(glm::vec2(16.0f, 16.0f), atlas1::container::kMinUV, atlas1::container::kMaxUV);
    first_ = false;
  }

  if (sprites_changed_) {
    UpdateVertexBuffer();
    UpdateIndexBuffer();
    UpdateSpritesBuffer();
    sprites_changed_ = false;
  }
  ASSERT(!sprites_changed_);

  if (sprites_.data.empty())
    return;

  GetPipeline()->Bind(&cmd);
  vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, *GetPipeline()->GetPipelineLayout(), 0, 1,
                          &GetDescriptors()->GetHandle(), 0, nullptr);
  vk::BindVertexBuffer(cmd, GetVertexBuffer());
  vk::BindIndexBuffer<uint16_t>(cmd, GetIndexBuffer());
  vkCmdDrawIndexed(cmd, SpriteMeshClass::kTotalNumberOfIndices, 1, 0, 0, 0);
}
}  // namespace prt