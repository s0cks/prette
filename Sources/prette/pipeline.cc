#include "prette/pipeline.h"

#include <fmt/format.h>
#include <vulkan/vulkan_core.h>

#include "prette/renderer.h"
#include "prette/shader.h"
#include "prette/swapchain.h"

namespace prt {
static inline void InitShaderStages(const Driver* driver, const std::string& shader, VkShaderModule& vert_shader,
                                    VkPipelineShaderStageCreateInfo& vert_stage, VkShaderModule& frag_shader,
                                    VkPipelineShaderStageCreateInfo& frag_stage) {
  ASSERT(driver);
  const auto vert_filename = fmt::format("{}.vert.spv", shader);
  LOG_IF(FATAL, !CreateShaderModule(driver->GetDevice(), vert_filename, vert_shader))
      << "failed to create vertex shader module for: " << vert_filename;
  InitVertexShaderStageCreateInfo(vert_stage, vert_shader);

  const auto frag_filename = fmt::format("{}.frag.spv", shader);
  LOG_IF(FATAL, !CreateShaderModule(driver->GetDevice(), frag_filename, frag_shader))
      << "failed to create fragment shader module for: " << frag_filename;
  InitFragmentShaderStageCreateInfo(frag_stage, frag_shader);
}

static inline void InitViewportAndScissor(VkViewport& viewport, VkRect2D& scissor, const VkExtent2D& extent,
                                          const glm::fvec2& pos = glm::fvec2(0.0f), const float min_depth = 0.0f,
                                          const float max_depth = 1.0f) {
  // viewport
  viewport.x = pos.x;
  viewport.y = pos.y;
  viewport.width = (float)extent.width;
  viewport.height = (float)extent.height;
  viewport.minDepth = min_depth;
  viewport.maxDepth = max_depth;
  // scissor
  scissor.offset = {.x = static_cast<int32_t>(pos.x), .y = static_cast<int32_t>(pos.y)};
  scissor.extent = extent;
}

static inline void InitViewportCreateInfo(VkPipelineViewportStateCreateInfo& create_info, const VkViewport& viewport,
                                          const VkRect2D& scissor) {
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  create_info.viewportCount = 1;
  create_info.pViewports = &viewport;
  create_info.scissorCount = 1;
  create_info.pScissors = &scissor;
}

static inline void InitRasterizer(VkPipelineRasterizationStateCreateInfo& rasterizer) {
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f;
  rasterizer.depthBiasClamp = 0.0f;
  rasterizer.depthBiasSlopeFactor = 0.0f;
}

static inline void InitMultisampling(VkPipelineMultisampleStateCreateInfo& multisampling) {
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f;
  multisampling.pSampleMask = nullptr;
  multisampling.alphaToCoverageEnable = VK_FALSE;
  multisampling.alphaToOneEnable = VK_FALSE;
}

static inline void InitDynamicState(VkPipelineDynamicStateCreateInfo& create_info,
                                    const std::vector<VkDynamicState>& dynamic_states) {
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  create_info.dynamicStateCount = dynamic_states.size();
  create_info.pDynamicStates = dynamic_states.data();
}

static inline void InitBlending(VkPipelineColorBlendStateCreateInfo& create_info,
                                const std::vector<VkPipelineColorBlendAttachmentState>& attachments) {
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  create_info.logicOpEnable = VK_FALSE;
  create_info.logicOp = VK_LOGIC_OP_COPY;
  create_info.attachmentCount = attachments.size();
  create_info.pAttachments = attachments.data();
  create_info.blendConstants[0] = 0.0f;
  create_info.blendConstants[1] = 0.0f;
  create_info.blendConstants[2] = 0.0f;
  create_info.blendConstants[3] = 0.0f;
}

static inline void InitInput(VkPipelineVertexInputStateCreateInfo& vertex_input,
                             VkPipelineInputAssemblyStateCreateInfo& input_assembly,
                             const std::array<VkVertexInputBindingDescription, 1>& bindings,
                             const std::array<VkVertexInputAttributeDescription, 3>& attributes) {
  vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input.vertexBindingDescriptionCount = bindings.size();
  vertex_input.pVertexBindingDescriptions = bindings.data();
  vertex_input.vertexAttributeDescriptionCount = attributes.size();
  vertex_input.pVertexAttributeDescriptions = attributes.data();

  input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;
}

static inline void InitColorBlendAttachment(VkPipelineColorBlendAttachmentState& color_blend_attachment) {
  color_blend_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_TRUE;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
}

static inline void InitDepthStencil(VkPipelineDepthStencilStateCreateInfo& depth_stencil) {
  depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil.depthTestEnable = VK_TRUE;
  depth_stencil.depthWriteEnable = VK_TRUE;
  depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depth_stencil.depthBoundsTestEnable = VK_FALSE;
  depth_stencil.minDepthBounds = 0.0f;
  depth_stencil.maxDepthBounds = 1.0f;
  depth_stencil.stencilTestEnable = VK_FALSE;
  depth_stencil.front = {};
  depth_stencil.back = {};
}

void GraphicsPipeline::Init(const Driver* driver, const std::string& shader, const std::vector<VkDynamicState>& dynamic_states,
                            const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts, const bool depth) {
  ASSERT(driver);
  VkPipelineLayoutCreateInfo pipeline_layout{};
  pipeline_layout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout.setLayoutCount = descriptor_set_layouts.size();
  pipeline_layout.pSetLayouts = descriptor_set_layouts.data();
  pipeline_layout.pushConstantRangeCount = 0;
  pipeline_layout.pPushConstantRanges = nullptr;
  CHECK_VK(FATAL, vkCreatePipelineLayout(driver->GetDevice(), &pipeline_layout, driver->GetAllocator(), &layout_),
           "failed to create vk pipeline layout");

  VkShaderModule vert_shader{};
  VkPipelineShaderStageCreateInfo vert_stage{};

  VkShaderModule frag_shader{};
  VkPipelineShaderStageCreateInfo frag_stage{};
  InitShaderStages(driver, shader, vert_shader, vert_stage, frag_shader, frag_stage);
  std::array<VkPipelineShaderStageCreateInfo, 2> stages = {
      vert_stage,
      frag_stage,
  };

  VkPipelineDepthStencilStateCreateInfo depth_stencil{};
  InitDepthStencil(depth_stencil);

  VkViewport viewport{};
  VkRect2D scissor{};
  InitViewportAndScissor(viewport, scissor, extent_);
  VkPipelineViewportStateCreateInfo viewport_create_info{};
  InitViewportCreateInfo(viewport_create_info, viewport, scissor);

  VkPipelineVertexInputStateCreateInfo vertex_input_info{};
  VkPipelineInputAssemblyStateCreateInfo input_assembly{};
  std::array<VkVertexInputBindingDescription, 1> bindings = {
      Vertex::GetBindingDescription(),
  };
  std::array<VkVertexInputAttributeDescription, 3> attributes = Vertex::GetAttributeDescriptions();
  InitInput(vertex_input_info, input_assembly, bindings, attributes);

  VkPipelineDynamicStateCreateInfo dynamic_state{};
  InitDynamicState(dynamic_state, dynamic_states);

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  InitRasterizer(rasterizer);
  if (shader == "scene") {
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  }

  VkPipelineMultisampleStateCreateInfo multisampling{};
  InitMultisampling(multisampling);

  VkPipelineColorBlendAttachmentState color_blend_attachment{};
  InitColorBlendAttachment(color_blend_attachment);

  VkPipelineColorBlendStateCreateInfo blending{};
  std::vector<VkPipelineColorBlendAttachmentState> attachments = {
      color_blend_attachment,
  };
  InitBlending(blending, attachments);

  VkGraphicsPipelineCreateInfo pipeline{};
  pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline.stageCount = 2;
  pipeline.pStages = &stages[0];
  pipeline.pVertexInputState = &vertex_input_info;
  pipeline.pInputAssemblyState = &input_assembly;
  pipeline.pViewportState = &viewport_create_info;
  pipeline.pRasterizationState = &rasterizer;
  pipeline.pMultisampleState = &multisampling;
  pipeline.pColorBlendState = &blending;
  pipeline.pDynamicState = &dynamic_state;
  pipeline.pDepthStencilState = &depth_stencil;
  pipeline.layout = layout_;
  pipeline.renderPass = pass_;
  pipeline.subpass = 0;
  pipeline.basePipelineHandle = VK_NULL_HANDLE;
  pipeline.basePipelineIndex = -1;

  ASSERT(driver);
  VkPipelineCacheCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
  CHECK_VK(FATAL, vkCreatePipelineCache(driver->GetDevice(), &create_info, driver->GetAllocator(), &cache_),
           "failed to create vk pipeline cache");
  CHECK_VK(FATAL, vkCreateGraphicsPipelines(driver->GetDevice(), cache_, 1, &pipeline, driver->GetAllocator(), &pipeline_),
           "failed to create vk pipeline");
  vkDestroyShaderModule(driver->GetDevice(), frag_shader, driver->GetAllocator());
  vkDestroyShaderModule(driver->GetDevice(), vert_shader, driver->GetAllocator());
}

void GraphicsPipeline::Destroy(const Driver* driver) {
  ASSERT(driver);
  vkDestroyPipelineLayout(driver->GetDevice(), layout_, driver->GetAllocator());
  vkDestroyPipelineCache(driver->GetDevice(), cache_, driver->GetAllocator());
  vkDestroyPipeline(driver->GetDevice(), pipeline_, driver->GetAllocator());
}

GraphicsPipeline::GraphicsPipeline(const Driver* driver, const std::string& shader, const VkRenderPass pass,
                                   const std::vector<VkDynamicState>& dynamic_states, const VkExtent2D& extent,
                                   const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts, const bool depth) :
  pass_(pass),
  extent_(extent) {
  Init(driver, shader, dynamic_states, descriptor_set_layouts, depth);
}
}  // namespace prt