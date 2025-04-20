#include "prette/pipeline.h"

#include <fmt/format.h>
#include <rapidjson/reader.h>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/pipeline_builder.h"
#include "prette/pipeline_json.h"
#include "prette/renderer.h"
#include "prette/shader.h"
#include "prette/swapchain.h"

namespace prt {
BaseRenderPipeline::~BaseRenderPipeline() {
  const auto driver = Driver::Get();
  vkDestroyPipelineLayout(driver->GetDevice(), layout_, driver->GetAllocator());
  vkDestroyPipelineCache(driver->GetDevice(), cache_, driver->GetAllocator());
  vkDestroyPipeline(driver->GetDevice(), pipeline_, driver->GetAllocator());
}

void BaseRenderPipelineBuilder::SetRasterizerCullMode(const std::string& rhs) {
  if (rhs.empty() || EqualsIgnoreCase(rhs, "none")) {
    rasterizer_.cullMode = VK_CULL_MODE_NONE;
  } else if (EqualsIgnoreCase(rhs, "front")) {
    rasterizer_.cullMode = VK_CULL_MODE_FRONT_BIT;
  } else if (EqualsIgnoreCase(rhs, "back")) {
    rasterizer_.cullMode = VK_CULL_MODE_BACK_BIT;
  } else if (EqualsIgnoreCase(rhs, "front-and-back")) {
    rasterizer_.cullMode = VK_CULL_MODE_FRONT_AND_BACK;
  }
}

void BaseRenderPipelineBuilder::SetRasterizerFrontFace(const std::string& rhs) {
  if (rhs.empty() || EqualsIgnoreCase(rhs, "clockwise") || EqualsIgnoreCase(rhs, "cw")) {
    rasterizer_.frontFace = VK_FRONT_FACE_CLOCKWISE;
  } else if (EqualsIgnoreCase(rhs, "counter-clockwise") || EqualsIgnoreCase(rhs, "ccw")) {
    rasterizer_.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  }
}

void BaseRenderPipelineBuilder::AttachVertexShader(ShaderPtr shader) {
  VkPipelineShaderStageCreateInfo create_info{};
  InitPipelineVertexShaderStage(create_info, shader);
  shader_attachments_.push_back(create_info);
  attached_shaders_.push_back(shader);
}

void BaseRenderPipelineBuilder::AttachFragmentShader(ShaderPtr shader) {
  VkPipelineShaderStageCreateInfo create_info{};
  InitPipelineFragmentShaderStage(create_info, shader);
  shader_attachments_.push_back(create_info);
  attached_shaders_.push_back(shader);
}

GraphicsPipeline::~GraphicsPipeline() {}

auto GraphicsPipeline::FromJson(fs::path path, VkRenderPass pass, VkExtent2D extent,
                                const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts) -> GraphicsPipeline* {
  DVLOG(1) << "parsing GraphicsPipeline from " << path << ".....";
  GraphicsPipelineBuilder builder(std::move(extent));
  json::PipelineHandler handler(&builder);
  json::ParseJsonDocumentFrom(path, handler);

  VkPipelineLayout layout{};
  PipelineLayoutBuilder layout_builder(descriptor_set_layouts);
  layout_builder.Build(layout);

  VkPipelineCache cache{};
  PipelineCacheBuilder cache_builder{};
  cache_builder.Build(cache);

  builder.SetRenderPass(pass);
  builder.SetPipelineLayout(layout);
  builder.SetPipelineCache(cache);
  return builder.Build();
}
}  // namespace prt