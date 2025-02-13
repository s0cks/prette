#include "prette/scene_renderer.h"

#include <vulkan/vulkan_core.h>

#include "prette/gfx.h"
#include "prette/gfx_vk.h"
#include "prette/gui.h"
#include "prette/renderer.h"
#include "prette/shader.h"

namespace prt {
static VkPipeline pipeline_{};
static VkPipelineLayout pipeline_layout_{};
static VkPipelineCache pipeline_cache_{};
static VkRenderPass pass_;
static std::vector<VkFramebuffer> framebuffers_{};
static VkCommandPool command_pool_{};
static std::array<VkCommandBuffer, MAX_NUMBER_OF_FRAMES_IN_FLIGHT> command_buffers_{};

static vk::Buffer* vertex_buffer_ = nullptr;
static vk::Buffer* index_buffer_ = nullptr;

static const std::vector<Vertex> vertices = {{.pos = {-0.5f, -0.5f}, .color = {1.0f, 0.0f, 0.0f}},
                                             {.pos = {0.5f, -0.5f}, .color = {0.0f, 1.0f, 0.0f}},
                                             {.pos = {0.5f, 0.5f}, .color = {0.0f, 0.0f, 1.0f}},
                                             {.pos = {-0.5f, 0.5f}, .color = {1.0f, 1.0f, 1.0f}}};

static const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

void SceneRenderer::InitRenderPass(const Driver* driver) {
  ASSERT(driver);
  VkAttachmentDescription color_attachment{};
  color_attachment.format = Renderer::GetFormat();
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_ref{};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref;

  VkSubpassDependency dependency1{};
  dependency1.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency1.dstSubpass = 0;
  dependency1.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  dependency1.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  dependency1.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency1.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependency1.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  VkSubpassDependency dependency2{};
  dependency1.srcSubpass = 0;
  dependency1.dstSubpass = VK_SUBPASS_EXTERNAL;
  dependency1.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency1.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependency1.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  dependency1.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  dependency1.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  std::array<VkSubpassDependency, 2> dependencies = {
      dependency1,
      dependency2,
  };

  VkRenderPassCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  create_info.attachmentCount = 1;
  create_info.pAttachments = &color_attachment;
  create_info.subpassCount = 1;
  create_info.pSubpasses = &subpass;
  create_info.dependencyCount = dependencies.size();
  create_info.pDependencies = dependencies.data();

  CHECK_VK(FATAL, vkCreateRenderPass(driver->GetDevice(), &create_info, nullptr, &pass_), "failed to create vk render pass");
}

void SceneRenderer::InitFramebuffers(const Driver* driver) {
  ASSERT(driver);
  vk::InitFramebuffers(driver, pass_, Renderer::GetImageViews(), Renderer::GetExtent(), framebuffers_);
}

void SceneRenderer::InitCommandPool(const Driver* driver) {
  ASSERT(driver);
  const auto indices = FindQueueFamilies(driver->GetPhysicalDevice(), driver->GetSurface());
  VkCommandPoolCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  create_info.queueFamilyIndex = indices.GetGraphicsFamily();
  CHECK_VK(FATAL, vkCreateCommandPool(driver->GetDevice(), &create_info, driver->GetAllocator(), &command_pool_),
           "failed to create vk command pool");
}

void SceneRenderer::InitCommandBuffers(const Driver* driver) {
  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = command_pool_;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = MAX_NUMBER_OF_FRAMES_IN_FLIGHT;
  CHECK_VK(FATAL, vkAllocateCommandBuffers(driver->GetDevice(), &alloc_info, &command_buffers_[0]),
           "failed to allocate vk command buffers");
}

void SceneRenderer::InitPipeline(const Driver* driver) {
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

void SceneRenderer::Draw(const uint32_t buffer_index, const uint32_t image_index, std::vector<VkCommandBuffer>& cmd_buffers) {
  auto& buffer = command_buffers_.at(buffer_index);
  CHECK_VK(FATAL, vkResetCommandBuffer(buffer, 0), "failed to reset vk command buffer");

  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  VkRenderPassBeginInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = pass_;
  render_pass_info.framebuffer = framebuffers_[image_index];
  render_pass_info.renderArea.offset = {.x = 0, .y = 0};
  render_pass_info.renderArea.extent = Renderer::GetExtent();

  VkClearValue clear_color{{0.0f, 0.0f, 0.0f, 1.0f}};
  render_pass_info.clearValueCount = 1;
  render_pass_info.pClearValues = &clear_color;

  CHECK_VK(FATAL, vkBeginCommandBuffer(buffer, &begin_info), "failed to begin command buffer recording");
  {
    vkCmdBeginRenderPass(buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
    {
      vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);
      VkViewport viewport{};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = (float)Renderer::GetExtent().width;
      viewport.height = (float)Renderer::GetExtent().height;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(buffer, 0, 1, &viewport);

      VkRect2D scissor{};
      scissor.offset = {.x = 0, .y = 0};
      scissor.extent = Renderer::GetExtent();
      vkCmdSetScissor(buffer, 0, 1, &scissor);

      VkBuffer vertex_buffers[] = {vertex_buffer_->GetBuffer()};
      VkDeviceSize offsets[] = {0};
      vkCmdBindVertexBuffers(buffer, 0, 1, vertex_buffers, offsets);

      vkCmdBindIndexBuffer(buffer, index_buffer_->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);
      vkCmdDrawIndexed(buffer, indices.size(), 1, 0, 0, 0);
    }

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), buffer);
    vkCmdEndRenderPass(buffer);
  }
  CHECK_VK(FATAL, vkEndCommandBuffer(buffer), "failed to end command buffer recording");
  cmd_buffers.push_back(buffer);
}

void SceneRenderer::InitBuffers() {
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

void SceneRenderer::Destroy(const Driver* driver, const bool is_reinit) {
  ASSERT(driver);
  if (!is_reinit) {
    DLOG(INFO) << "destroying....";
    vkDestroyPipelineLayout(driver->GetDevice(), pipeline_layout_, driver->GetAllocator());
    vkDestroyPipelineCache(driver->GetDevice(), pipeline_cache_, driver->GetAllocator());
    vkDestroyPipeline(driver->GetDevice(), pipeline_, driver->GetAllocator());
    vkDestroyRenderPass(driver->GetDevice(), pass_, driver->GetAllocator());
  }
  for (const auto& framebuffer : framebuffers_) {
    vkDestroyFramebuffer(driver->GetDevice(), framebuffer, driver->GetAllocator());
  }
  if (!is_reinit) {
    ASSERT(vertex_buffer_);
    vertex_buffer_->Destroy();
    ASSERT(index_buffer_);
    index_buffer_->Destroy();
    vkFreeCommandBuffers(driver->GetDevice(), command_pool_, command_buffers_.size(), command_buffers_.data());
    vkDestroyCommandPool(driver->GetDevice(), command_pool_, driver->GetAllocator());
  }
}

auto SceneRenderer::GetRenderPass() -> VkRenderPass const& {
  return pass_;
}

auto SceneRenderer::GetCommandPool() -> VkCommandPool const& {
  return command_pool_;
}

void SceneRenderer::Init() {
  OnSwapChainInitEvent().subscribe([](SwapChainInitEvent* event) {
    ASSERT(event);
    const auto driver = Driver::Get();
    ASSERT(driver);
    if (!event->IsReinit()) {
      InitRenderPass(driver);
    }
    InitFramebuffers(driver);
    if (!event->IsReinit()) {
      InitPipeline(driver);
      InitCommandPool(driver);
      InitCommandBuffers(driver);
      InitBuffers();
    }
  });
  OnSwapChainDestroyedEvent().subscribe([](SwapChainDestroyedEvent* event) {
    ASSERT(event);
    const auto driver = Driver::Get();
    ASSERT(driver);
    Destroy(driver, event->IsReinit());
  });
}
}  // namespace prt