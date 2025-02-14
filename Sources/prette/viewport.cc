#include "prette/viewport.h"

#include <vector>

#include "prette/common.h"
#include "prette/gfx.h"
#include "prette/gfx_vk.h"
#include "prette/renderer.h"
#include "prette/shader.h"
#include "prette/swapchain.h"

namespace prt {
static std::vector<VkImage> images_{};
static std::vector<VkDeviceMemory> memory_{};
static std::vector<VkImageView> views_{};
static VkRenderPass pass_{};
static VkPipeline pipeline_{};
static VkPipelineLayout pipeline_layout_{};
static VkPipelineCache pipeline_cache_{};
static VkCommandPool pool_{};
static std::vector<VkFramebuffer> framebuffers_{};
static std::vector<VkCommandBuffer> command_buffers_{};

void Viewport::InitRenderPass(const Driver* driver) {
  std::array<VkAttachmentDescription, 1> attachments = {};
  // Color attachment
  attachments[0].format = SwapChain::GetFormat();
  attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
  attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attachments[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  VkAttachmentReference colorReference = {};
  colorReference.attachment = 0;
  colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthReference = {};
  depthReference.attachment = 1;
  depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpassDescription = {};
  subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpassDescription.colorAttachmentCount = 1;
  subpassDescription.pColorAttachments = &colorReference;
  subpassDescription.pDepthStencilAttachment = &depthReference;
  subpassDescription.inputAttachmentCount = 0;
  subpassDescription.pInputAttachments = nullptr;
  subpassDescription.preserveAttachmentCount = 0;
  subpassDescription.pPreserveAttachments = nullptr;
  subpassDescription.pResolveAttachments = nullptr;

  // Subpass dependencies for layout transitions
  std::array<VkSubpassDependency, 2> dependencies{};
  dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
  dependencies[0].dstSubpass = 0;
  dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  dependencies[1].srcSubpass = 0;
  dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
  dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpassDescription;
  renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
  renderPassInfo.pDependencies = dependencies.data();
  CHECK_VK(FATAL, vkCreateRenderPass(driver->GetDevice(), &renderPassInfo, nullptr, &pass_), "failed to create vk render pass");
}

void Viewport::InitPipeline(const Driver* driver) {
  ASSERT(driver);
  static constexpr const auto kVertexShaderFilename = "test.vert.spv";
  VkShaderModule vert_module = VK_NULL_HANDLE;
  LOG_IF(FATAL, !CreateShaderModule(driver->GetDevice(), kVertexShaderFilename, vert_module))
      << "failed to create vertex shader module for: " << kVertexShaderFilename;

  static constexpr const auto kFragmentShaderFilename = "test.frag.spv";
  VkShaderModule frag_module = VK_NULL_HANDLE;
  LOG_IF(FATAL, !CreateShaderModule(driver->GetDevice(), kFragmentShaderFilename, frag_module))
      << "failed to create fragment shader module for: " << kFragmentShaderFilename;

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
  pipeline.renderPass = pass_;
  pipeline.subpass = 0;
  pipeline.basePipelineHandle = VK_NULL_HANDLE;

  CHECK_VK(FATAL, vkCreateGraphicsPipelines(driver->GetDevice(), VK_NULL_HANDLE, 1, &pipeline, nullptr, &pipeline_),
           "failed to create vk pipeline");

  vkDestroyShaderModule(driver->GetDevice(), frag_module, nullptr);
  vkDestroyShaderModule(driver->GetDevice(), vert_module, nullptr);

  ASSERT(driver);
  VkPipelineCacheCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
  CHECK_VK(FATAL, vkCreatePipelineCache(driver->GetDevice(), &create_info, driver->GetAllocator(), &pipeline_cache_),
           "failed to create vk pipeline cache");
}

void Viewport::InitImage(const Driver* driver) {
  ASSERT(driver);
}

void Viewport::InitImageViews(const Driver* driver) {
  ASSERT(driver);
}

void Viewport::InitFramebuffers(const Driver* driver) {
  ASSERT(driver);
  vk::InitFramebuffers(driver, pass_, views_, SwapChain::GetExtent(), framebuffers_);
}

void Viewport::Init() {}
}  // namespace prt