#include "prette/pipeline.h"

#include <fmt/format.h>
#include <vulkan/vulkan_core.h>

#include <vector>

#include "prette/command_pool.h"
#include "prette/flags.h"
#include "prette/gfx.h"
#include "prette/renderer.h"
#include "prette/shader.h"
#include "prette/swap_chain.h"

namespace prt {
static VkPipelineLayout pipeline_layout_{};
static VkPipeline pipeline_{};

static vk::Buffer* vertex_buffer_ = nullptr;
static vk::Buffer* index_buffer_ = nullptr;

static const std::vector<Vertex> vertices = {{.pos = {-0.5f, -0.5f}, .color = {1.0f, 0.0f, 0.0f}},
                                             {.pos = {0.5f, -0.5f}, .color = {0.0f, 1.0f, 0.0f}},
                                             {.pos = {0.5f, 0.5f}, .color = {0.0f, 0.0f, 1.0f}},
                                             {.pos = {-0.5f, 0.5f}, .color = {1.0f, 1.0f, 1.0f}}};

static const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

void Pipeline::Init(Driver* driver) {
  ASSERT(driver);
  std::vector<char> vert_code{};
  ReadShaderCode(fmt::format("{}/shaders/vert.spv", FLAGS_resources), vert_code);
  VkShaderModule vert_module = VK_NULL_HANDLE;
  CreateShaderModule(driver->GetDevice(), vert_code, vert_module);

  std::vector<char> frag_code{};
  ReadShaderCode(fmt::format("{}/shaders/frag.spv", FLAGS_resources), frag_code);
  VkShaderModule frag_module = VK_NULL_HANDLE;
  CreateShaderModule(driver->GetDevice(), frag_code, frag_module);

  VkPipelineShaderStageCreateInfo vert_stage{};
  InitVertexShaderStageCreateInfo(vert_stage, vert_module);

  VkPipelineShaderStageCreateInfo frag_stage{};
  InitFragmentShaderStageCreateInfo(frag_stage, frag_module);

  std::vector<VkPipelineShaderStageCreateInfo> stages = {
      vert_stage,
      frag_stage,
  };

  const auto bindings = Vertex::GetBindingDescription();
  const auto attributes = Vertex::GetAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo vertex_input_info{};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount = 1;
  vertex_input_info.pVertexBindingDescriptions = &bindings;

  vertex_input_info.vertexAttributeDescriptionCount = attributes.size();
  vertex_input_info.pVertexAttributeDescriptions = attributes.data();

  VkPipelineInputAssemblyStateCreateInfo input_assembly{};
  input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  VkPipelineViewportStateCreateInfo viewport_state{};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendAttachmentState color_blend_attachment{};
  color_blend_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo color_blend{};
  color_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend.logicOpEnable = VK_FALSE;
  color_blend.logicOp = VK_LOGIC_OP_COPY;
  color_blend.attachmentCount = 1;
  color_blend.pAttachments = &color_blend_attachment;
  color_blend.blendConstants[0] = 0.0f;
  color_blend.blendConstants[1] = 0.0f;
  color_blend.blendConstants[2] = 0.0f;
  color_blend.blendConstants[3] = 0.0f;

  std::vector<VkDynamicState> dynamic_states = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR,
  };
  VkPipelineDynamicStateCreateInfo dynamic_state{};
  dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state.dynamicStateCount = dynamic_states.size();
  dynamic_state.pDynamicStates = &dynamic_states[0];

  VkPipelineLayoutCreateInfo pipeline_layout{};
  pipeline_layout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout.setLayoutCount = 0;
  pipeline_layout.pushConstantRangeCount = 0;

  CHECK_VK(FATAL, vkCreatePipelineLayout(driver->GetDevice(), &pipeline_layout, nullptr, &pipeline_layout_),
           "failed to create vk pipeline layout");

  VkGraphicsPipelineCreateInfo pipeline{};
  pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline.stageCount = 2;
  pipeline.pStages = &stages[0];
  pipeline.pVertexInputState = &vertex_input_info;
  pipeline.pInputAssemblyState = &input_assembly;
  pipeline.pViewportState = &viewport_state;
  pipeline.pRasterizationState = &rasterizer;
  pipeline.pMultisampleState = &multisampling;
  pipeline.pColorBlendState = &color_blend;
  pipeline.pDynamicState = &dynamic_state;
  pipeline.layout = pipeline_layout_;
  pipeline.renderPass = SwapChain::GetRenderPass();
  pipeline.subpass = 0;
  pipeline.basePipelineHandle = VK_NULL_HANDLE;

  CHECK_VK(FATAL, vkCreateGraphicsPipelines(driver->GetDevice(), VK_NULL_HANDLE, 1, &pipeline, nullptr, &pipeline_),
           "failed to create vk pipeline");

  vkDestroyShaderModule(driver->GetDevice(), frag_module, nullptr);
  vkDestroyShaderModule(driver->GetDevice(), vert_module, nullptr);
}

void Pipeline::InitBuffers() {
  {
    // index buffer
    const VkDeviceSize buffer_size = (sizeof(uint16_t) * indices.size());
    index_buffer_ = vk::Buffer::New(buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    ASSERT(index_buffer_);
    index_buffer_->CopyFromBytes(&indices[0], buffer_size, true);
  }

  {
    // vertex buffer
    const VkDeviceSize buffer_size = (sizeof(Vertex) * vertices.size());
    vertex_buffer_ = vk::Buffer::New(buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    ASSERT(vertex_buffer_);
    vertex_buffer_->CopyFromBytes(&vertices[0], buffer_size, true);
  }
}

void Pipeline::DestroyPipeline(Driver* driver) {
  ASSERT(driver);
  vkDestroyPipeline(driver->GetDevice(), pipeline_, driver->GetAllocator());
}

void Pipeline::DestroyPipelineLayout(Driver* driver) {
  ASSERT(driver);
  vkDestroyPipelineLayout(driver->GetDevice(), pipeline_layout_, driver->GetAllocator());
}

auto Pipeline::GetPipeline() -> const VkPipeline& {
  return pipeline_;
}

auto Pipeline::GetPipelineLayout() -> const VkPipelineLayout& {
  return pipeline_layout_;
}

auto Pipeline::GetVertexBuffer() -> const VkBuffer& {
  ASSERT(vertex_buffer_);
  return vertex_buffer_->GetBuffer();
}

auto Pipeline::GetIndexBuffer() -> const VkBuffer& {
  ASSERT(index_buffer_);
  return index_buffer_->GetBuffer();
}

auto Pipeline::GetNumberOfIndices() -> uint32_t {
  return indices.size();
}

void Pipeline::Shutdown(Driver* driver) {
  ASSERT(driver);
  DestroyPipeline(driver);
  DestroyPipelineLayout(driver);
  ASSERT(index_buffer_);
  index_buffer_->Destroy();
  ASSERT(vertex_buffer_);
  vertex_buffer_->Destroy();
}
}  // namespace prt