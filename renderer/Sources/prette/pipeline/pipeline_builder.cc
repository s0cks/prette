#include "prette/pipeline/pipeline_builder.h"

#include <string>
#include <utility>
#include <vector>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/pipeline/pipeline.h"
#include "prette/pipeline/pipeline_layout.h"
#include "prette/shader/shader.h"
#include "prette/vk.h"

namespace prt::vk {
void BaseRenderPipelineBuilder::SetRasterizerCullMode(const VkCullModeFlags rhs) {
  rasterizer_.cullMode = rhs;
}

void BaseRenderPipelineBuilder::SetRasterizerPolygonMode(const VkPolygonMode rhs) {
  rasterizer_.polygonMode = rhs;
}

void BaseRenderPipelineBuilder::SetRasterizerCullMode(const std::string& rhs) {
  if (rhs.empty() || EqualsIgnoreCase(rhs, "none")) {
    return SetRasterizerCullMode(VK_CULL_MODE_NONE);
  } else if (EqualsIgnoreCase(rhs, "front")) {
    return SetRasterizerCullMode(VK_CULL_MODE_FRONT_BIT);
  } else if (EqualsIgnoreCase(rhs, "back")) {
    return SetRasterizerCullMode(VK_CULL_MODE_BACK_BIT);
  } else if (EqualsIgnoreCase(rhs, "front-and-back")) {
    return SetRasterizerCullMode(VK_CULL_MODE_FRONT_AND_BACK);
  }
}

void BaseRenderPipelineBuilder::SetLineWidth(const float rhs) {
  rasterizer_.lineWidth = rhs;
}

void BaseRenderPipelineBuilder::SetRasterizerFrontFace(const VkFrontFace rhs) {
  rasterizer_.frontFace = rhs;
}

void BaseRenderPipelineBuilder::SetRasterizerFrontFace(const std::string& rhs) {
  if (rhs.empty() || EqualsIgnoreCase(rhs, "clockwise") || EqualsIgnoreCase(rhs, "cw")) {
    return SetRasterizerFrontFace(VK_FRONT_FACE_CLOCKWISE);
  } else if (EqualsIgnoreCase(rhs, "counter-clockwise") || EqualsIgnoreCase(rhs, "ccw")) {
    return SetRasterizerFrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE);
  }
}

void BaseRenderPipelineBuilder::AttachVertexShader(Shader* shader) {
  auto stage = WithVertexShaderStage().WithModule(shader->GetHandle()).WithName("main");
  attached_shaders_.push_back(shader);
}

void BaseRenderPipelineBuilder::AttachFragmentShader(Shader* shader) {
  auto stage = WithFragmentShaderStage().WithModule(shader->GetHandle()).WithName("main");
  attached_shaders_.push_back(shader);
}

void BaseRenderPipelineBuilder::SetPipelineLayout(vk::PipelineLayout* rhs) {
  ASSERT(rhs && rhs->IsInitialized());
  layout_ = std::move(rhs);
}

void BaseRenderPipelineBuilder::InitPipelineInfo(VkGraphicsPipelineCreateInfo& info) {
  InitInput(vertex_input_, input_assembly_, vertex_bindings_, vertex_attrs_);
  info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  info.stageCount = shader_stages_.size();
  info.pStages = shader_stages_.data();
  info.pVertexInputState = &vertex_input_;
  info.pInputAssemblyState = &input_assembly_;
  info.pViewportState = &viewport_;
  info.pRasterizationState = &rasterizer_;
  info.pMultisampleState = &multisampling_;
  info.pColorBlendState = &blending_;
  info.pDynamicState = &dynamic_state_;
  info.pDepthStencilState = &depth_stencil_;
  info.layout = layout_ ? layout_->GetHandle() : VK_NULL_HANDLE;
  info.renderPass = render_pass_;
  info.subpass = 0;
  info.basePipelineHandle = VK_NULL_HANDLE;
  info.basePipelineIndex = -1;
}

void BaseRenderPipelineBuilder::AddVertexBindings(const VkVertexInputBindingDescription* data,
                                                  const uint64_t num_bindings) {
  ASSERT(num_bindings > 0);
  vertex_bindings_.insert(std::end(vertex_bindings_), data, data + num_bindings);
}

void BaseRenderPipelineBuilder::AddVertexAttributes(const VkVertexInputAttributeDescription* data,
                                                    const uint64_t num_attrs) {
  ASSERT(num_attrs > 0);
  vertex_attrs_.insert(std::end(vertex_attrs_), data, data + num_attrs);
}

auto RenderPipelineBuilder::Build() -> RenderPipeline* {
  ASSERT(shader_stages_.size() >= 2);
  InitPipelineInfo(pipeline_);
  return new RenderPipeline(std::move(name_), extent_, pipeline_, layout_, cache_);
}
}  // namespace prt::vk