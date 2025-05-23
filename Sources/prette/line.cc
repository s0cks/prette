#include "prette/line.h"

#include <array>

#include "prette/common.h"
#include "prette/descriptor_set.h"
#include "prette/descriptor_set_builder.h"
#include "prette/pipeline.h"
#include "prette/pipeline_layout.h"
#include "prette/shader.h"
#include "prette/swapchain.h"
#include "prette/vk.h"
#include "prette/vk_buffer.h"

namespace prt {
static constexpr const auto kTotalNumberOfVerticesPerLine = 2;
auto LineRenderer::CreateVertexBuffer(const uint64_t num_lines) -> vk::Buffer* {
  ASSERT(num_lines > 0);
  const auto total_size = sizeof(LineVertex) * kTotalNumberOfVerticesPerLine * num_lines;
  return vk::Buffer::New(total_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
}

static constexpr const auto kTotalNumberOfIndicesPerLine = 2;
auto LineRenderer::CreateIndexBuffer(const uint64_t num_lines) -> vk::Buffer* {
  ASSERT(num_lines > 0);
  const auto total_size = sizeof(LineRenderer::IndexType) * kTotalNumberOfVerticesPerLine * num_lines;
  return vk::Buffer::New(total_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
}

auto LineRenderer::CreateDescriptorSet() -> vk::DescriptorSet* {
  vk::DescriptorSetBuilder builder;
  return builder.Build();
}

LineRenderer::LineRenderer(const VkRenderPass& pass, const uint64_t max_lines) :
  max_lines_(max_lines),
  vertices_(CreateVertexBuffer(max_lines)),
  indices_(CreateIndexBuffer(max_lines)),
  descriptors_(CreateDescriptorSet()) {
  ASSERT(max_lines > 0);
  ASSERT(vertices_);
  ASSERT(indices_);
  UpdateDescriptorSet();

  vk::PipelineLayout* layout = nullptr;
  {
    std::array<VkDescriptorSetLayout, 1> descriptor_set_layouts = {
        descriptors_->GetLayout(),
    };
    vk::PipelineLayoutBuilder builder{};
    builder.WithName("line").WithDescriptorSetLayouts(descriptor_set_layouts);
    layout = builder.Build();
    ASSERT(layout && layout->IsInitialized());
  }

  const auto& extent = GetSwapchain()->GetExtent();
  vk::RenderPipelineBuilder builder(extent);
  builder.SetRenderPass(pass);
  builder.SetPipelineLayout(layout);
  builder.SetRasterizerCullMode(VK_CULL_MODE_BACK_BIT);
  builder.SetRasterizerFrontFace(VK_FRONT_FACE_CLOCKWISE);

  const auto vtx_shader = vk::GetVertexShader("tile");
  const auto frag_shader = vk::GetFragmentShader("tile");

  builder.AttachFragmentShader(frag_shader);
  builder.AttachVertexShader(vtx_shader);
  pipeline_ = builder.Build();

  delete vtx_shader;
  delete frag_shader;
}

void LineRenderer::Render(VkCommandBuffer& cmd) {
  std::array<VkDeviceSize, 1> offsets = {0};
  vkCmdBindVertexBuffers(cmd, 0, 1, &GetVertexBuffer()->GetBuffer(), offsets.data());
  vkCmdBindIndexBuffer(cmd, GetIndexBuffer()->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);
  vkCmdDrawIndexed(cmd, 2, GetNumberOfLines(), 0, 0, 0);
}

void LineRenderer::UpdateDescriptorSet() {}
}  // namespace prt