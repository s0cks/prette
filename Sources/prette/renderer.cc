#include "prette/renderer.h"

#include "prette/engine.h"
#include "prette/gfx.h"
#include "prette/lua.h"
#include "prette/relaxed_atomic.h"
#include "prette/shader.h"
#include "prette/to_string.h"
#include "prette/window.h"

namespace prt {
static RendererEventSubject events_{};
static std::array<VkSemaphore, MAX_NUMBER_OF_FRAMES_IN_FLIGHT> available_semaphores_{};
static std::array<VkSemaphore, MAX_NUMBER_OF_FRAMES_IN_FLIGHT> finished_semaphores_{};
static std::array<VkFence, MAX_NUMBER_OF_FRAMES_IN_FLIGHT> fences_{};
static RelaxedAtomic<uint32_t> current_frame_(0);
static RelaxedAtomic<bool> resized_(false);

static VkPipelineCache pipeline_cache_{};
static VkPipelineLayout pipeline_layout_{};
static VkPipeline pipeline_{};

static VkCommandPool command_pool_{};
static std::array<VkCommandBuffer, MAX_NUMBER_OF_FRAMES_IN_FLIGHT> command_buffers_{};

static vk::Buffer* vertex_buffer_ = nullptr;
static vk::Buffer* index_buffer_ = nullptr;

static VkSwapchainKHR chain_{};
static std::vector<VkImage> images_{};
static std::vector<VkImageView> views_{};
static std::vector<VkFramebuffer> framebuffers_{};
static VkRenderPass render_pass_;
static VkFormat format_{};
static VkExtent2D extent_{};

static const std::vector<Vertex> vertices = {{.pos = {-0.5f, -0.5f}, .color = {1.0f, 0.0f, 0.0f}},
                                             {.pos = {0.5f, -0.5f}, .color = {0.0f, 1.0f, 0.0f}},
                                             {.pos = {0.5f, 0.5f}, .color = {0.0f, 0.0f, 1.0f}},
                                             {.pos = {-0.5f, 0.5f}, .color = {1.0f, 1.0f, 1.0f}}};

static const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

void Renderer::InitPipelineCache(const Driver* driver) {
  ASSERT(driver);
  VkPipelineCacheCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
  CHECK_VK(FATAL, vkCreatePipelineCache(driver->GetDevice(), &create_info, driver->GetAllocator(), &pipeline_cache_),
           "failed to create vk pipeline cache");
}

void Renderer::InitPipeline(const Driver* driver) {
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
  pipeline.renderPass = GetRenderPass();
  pipeline.subpass = 0;
  pipeline.basePipelineHandle = VK_NULL_HANDLE;

  CHECK_VK(FATAL, vkCreateGraphicsPipelines(driver->GetDevice(), VK_NULL_HANDLE, 1, &pipeline, nullptr, &pipeline_),
           "failed to create vk pipeline");

  vkDestroyShaderModule(driver->GetDevice(), frag_module, nullptr);
  vkDestroyShaderModule(driver->GetDevice(), vert_module, nullptr);
}

static inline void PublishRendererEvent(RendererEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

template <class E, typename... Args>
static inline void Publish(Args... args) {
  E event(args...);
  return PublishRendererEvent(&event);
}

auto OnRendererEvent() -> RendererEventObservable {
  return events_.get_observable();
}

auto PreFrameEvent::ToString() const -> std::string {
  ToStringHelper<PreFrameEvent> helper{};
  return helper;
}

auto PostFrameEvent::ToString() const -> std::string {
  ToStringHelper<PostFrameEvent> helper{};
  return helper;
}

auto RendererCreatedEvent::ToString() const -> std::string {
  ToStringHelper<RendererCreatedEvent> helper{};
  return helper;
}

auto RendererDestroyedEvent::ToString() const -> std::string {
  ToStringHelper<RendererDestroyedEvent> helper{};
  return helper;
}

void Renderer::InitResizeListener() {
  const auto window = GetAppWindow();
  ASSERT(window);
  window->OnWindowSizeEvent().subscribe([](WindowSizeEvent* event) {
    ASSERT(event);
    resized_ = true;
  });
}

void Renderer::InitSyncObjects(const Driver* driver) {
  ASSERT(driver);
  VkSemaphoreCreateInfo semaphore_info{};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fence_info{};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (auto idx = 0; idx < MAX_NUMBER_OF_FRAMES_IN_FLIGHT; idx++) {
    CHECK_VK(FATAL,
             vkCreateSemaphore(driver->GetDevice(), &semaphore_info, driver->GetAllocator(), &available_semaphores_.at(idx)),
             "failed to create available semaphore");
    CHECK_VK(FATAL,
             vkCreateSemaphore(driver->GetDevice(), &semaphore_info, driver->GetAllocator(), &finished_semaphores_.at(idx)),
             "failed to create finished semaphore");
    CHECK_VK(FATAL, vkCreateFence(driver->GetDevice(), &fence_info, driver->GetAllocator(), &fences_.at(idx)),
             "failed to create fence");
  }
}

void Renderer::DestroySyncObjects(const Driver* driver) {
  ASSERT(driver);
  for (auto idx = 0; idx < MAX_NUMBER_OF_FRAMES_IN_FLIGHT; idx++) {
    vkDestroySemaphore(driver->GetDevice(), finished_semaphores_.at(idx), driver->GetAllocator());
    vkDestroySemaphore(driver->GetDevice(), available_semaphores_.at(idx), driver->GetAllocator());
    vkDestroyFence(driver->GetDevice(), fences_.at(idx), driver->GetAllocator());
  }
}

static inline auto GetCurrentFrame() -> uint32_t {
  return (uint32_t)current_frame_;
}

static inline auto GetCurrentFence() -> const VkFence& {
  return fences_.at(GetCurrentFrame());
}

static inline auto GetCurrentAvailableSemaphore() -> const VkSemaphore& {
  return available_semaphores_.at(GetCurrentFrame());
}

static inline auto GetCurrentFinishedSemaphore() -> const VkSemaphore& {
  return finished_semaphores_.at(GetCurrentFrame());
}

void Renderer::InitBuffers() {
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

void Renderer::DestroyPipeline(const Driver* driver) {
  ASSERT(driver);
  vkDestroyPipeline(driver->GetDevice(), pipeline_, driver->GetAllocator());
}

void Renderer::DestroyPipelineLayout(const Driver* driver) {
  ASSERT(driver);
  vkDestroyPipelineLayout(driver->GetDevice(), pipeline_layout_, driver->GetAllocator());
}

void Renderer::DestroyPipelineCache(const Driver* driver) {
  ASSERT(driver);
  vkDestroyPipelineCache(driver->GetDevice(), pipeline_cache_, driver->GetAllocator());
}

auto Renderer::GetPipeline() -> const VkPipeline& {
  return pipeline_;
}

auto Renderer::GetPipelineLayout() -> const VkPipelineLayout& {
  return pipeline_layout_;
}

auto Renderer::GetVertexBuffer() -> const VkBuffer& {
  ASSERT(vertex_buffer_);
  return vertex_buffer_->GetBuffer();
}

auto Renderer::GetIndexBuffer() -> const VkBuffer& {
  ASSERT(index_buffer_);
  return index_buffer_->GetBuffer();
}

auto Renderer::GetNumberOfIndices() -> uint32_t {
  return indices.size();
}

auto Renderer::GetPipelineCache() -> VkPipelineCache const& {
  return pipeline_cache_;
}

auto Renderer::GetCommandPool() -> const VkCommandPool& {
  return command_pool_;
}

void Renderer::InitCommandPool(const VkPhysicalDevice& physical_device, const VkDevice& device, const VkSurfaceKHR& surface,
                               const VkAllocationCallbacks* allocator) {
  const auto indices = FindQueueFamilies(physical_device, surface);
  VkCommandPoolCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  create_info.queueFamilyIndex = indices.GetGraphicsFamily();
  CHECK_VK(FATAL, vkCreateCommandPool(device, &create_info, allocator, &command_pool_), "failed to create vk command pool");
}

void Renderer::InitCommandBuffers(const VkDevice& device) {
  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = command_pool_;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = command_buffers_.size();
  CHECK_VK(FATAL, vkAllocateCommandBuffers(device, &alloc_info, &command_buffers_[0]), "failed to allocate vk command buffers");
}

auto Renderer::GetCommandBuffer(const uint32_t buffer_index) -> const VkCommandBuffer& {
  return command_buffers_.at(buffer_index);
}

void Renderer::ResetCommandBuffer(const uint32_t buffer_index, const VkCommandBufferResetFlagBits flags) {
  CHECK_VK(FATAL, vkResetCommandBuffer(GetCommandBuffer(buffer_index), flags), "failed to reset vk command buffer");
}

void Renderer::RecordCommandBuffer(const uint32_t buffer_index, const uint32_t image_index) {
  auto& buffer = GetCommandBuffer(buffer_index);
  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  CHECK_VK(FATAL, vkBeginCommandBuffer(buffer, &begin_info), "failed to begin command buffer recording");
  {
    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = Renderer::GetRenderPass();
    render_pass_info.framebuffer = Renderer::GetFramebuffer(image_index);
    render_pass_info.renderArea.offset = {.x = 0, .y = 0};
    render_pass_info.renderArea.extent = Renderer::GetExtent();

    VkClearValue clear_color{{0.0f, 0.0f, 0.0f, 1.0f}};
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass(buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
    {
      vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipeline());
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

      VkBuffer vertex_buffers[] = {GetVertexBuffer()};
      VkDeviceSize offsets[] = {0};
      vkCmdBindVertexBuffers(buffer, 0, 1, vertex_buffers, offsets);

      vkCmdBindIndexBuffer(buffer, GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);

      vkCmdDrawIndexed(buffer, GetNumberOfIndices(), 1, 0, 0, 0);
    }
    vkCmdEndRenderPass(buffer);
  }
  CHECK_VK(FATAL, vkEndCommandBuffer(buffer), "failed to end command buffer recording");
}

auto QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> SwapChainSupportDetails {
  SwapChainSupportDetails details{};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.surface_capabilities);

  uint32_t num_formats = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &num_formats, nullptr);
  if (num_formats > 0) {
    details.surface_formats.resize(num_formats);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &num_formats, &details.surface_formats[0]);
  }

  uint32_t num_present_modes = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &num_present_modes, nullptr);
  if (num_present_modes > 0) {
    details.present_modes.resize(num_present_modes);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &num_present_modes, &details.present_modes[0]);
  }
  return details;
}

auto SwapChainSupportDetails::GetExtent() const -> VkExtent2D {
  if (surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return surface_capabilities.currentExtent;
  }

  const auto window = GetAppWindow();
  ASSERT(window);
  VkExtent2D actual = window->GetFramebufferSize();
  return ClampExtent(actual, surface_capabilities);
}

static inline auto IsValidSwapChainSurfaceFormat(const VkSurfaceFormatKHR& format) -> bool {
  return format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
}

static inline auto IsValidSwapChainPresentMode(const VkPresentModeKHR& mode) -> bool {
  return mode == VK_PRESENT_MODE_MAILBOX_KHR;
}

auto Renderer::GetRenderPass() -> VkRenderPass const& {
  return render_pass_;
}

auto Renderer::GetFramebuffer(const uint32_t frame) -> VkFramebuffer const& {
  return framebuffers_.at(frame);
}

auto Renderer::GetSwapChain() -> VkSwapchainKHR const& {
  return chain_;
}

auto Renderer::GetExtent() -> VkExtent2D const& {
  return extent_;
}

void Renderer::InitRenderPass(const VkDevice& device) {
  VkAttachmentDescription color_attachment{};
  color_attachment.format = format_;
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

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  create_info.attachmentCount = 1;
  create_info.pAttachments = &color_attachment;
  create_info.subpassCount = 1;
  create_info.pSubpasses = &subpass;
  create_info.dependencyCount = 1;
  create_info.pDependencies = &dependency;

  CHECK_VK(FATAL, vkCreateRenderPass(device, &create_info, nullptr, &render_pass_), "failed to create vk render pass");
}

void Renderer::InitImageViews(const VkDevice& device) {
  views_.resize(images_.size());
  for (auto idx = 0; idx < images_.size(); idx++) {
    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = images_[idx];
    create_info.format = format_;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;
    CHECK_VK(FATAL, vkCreateImageView(device, &create_info, nullptr, &views_[idx]), "failed to create vk image view");
  }
}

void Renderer::InitFramebuffers(const VkDevice& device, const VkAllocationCallbacks* allocator) {
  framebuffers_.resize(views_.size());
  for (auto idx = 0; idx < views_.size(); idx++) {
    VkFramebufferCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    create_info.renderPass = GetRenderPass();
    create_info.attachmentCount = 1;
    create_info.pAttachments = &views_[idx];
    create_info.width = GetExtent().width;
    create_info.height = GetExtent().height;
    create_info.layers = 1;
    CHECK_VK(FATAL, vkCreateFramebuffer(device, &create_info, allocator, &framebuffers_[idx]),
             "failed to create vk framebuffers");
  }
}

void Renderer::InitSwapChain(const VkPhysicalDevice& physical_device, const VkDevice& device, const VkSurfaceKHR& surface,
                             const VkAllocationCallbacks* allocator) {
  const auto support = QuerySwapChainSupport(physical_device, surface);
  const auto surface_format = support.FindSurfaceFormat(&IsValidSwapChainSurfaceFormat);
  const auto present_mode = support.FindPresentMode(&IsValidSwapChainPresentMode);
  const auto extent = support.GetExtent();

  uint32_t image_count = support.surface_capabilities.minImageCount + 1;
  support.ClampImageCount(image_count);

  VkSwapchainCreateInfoKHR create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = surface;
  create_info.minImageCount = image_count;
  create_info.imageFormat = surface_format.format;
  create_info.imageColorSpace = surface_format.colorSpace;
  create_info.imageExtent = extent;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  const auto indices = FindQueueFamilies(physical_device, surface);
  const std::array<uint32_t, 2> families = {
      indices.GetGraphicsFamily(),
      indices.GetPresentFamily(),
  };
  if (indices.GetGraphicsFamily() != indices.GetPresentFamily()) {
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = &families[0];
  } else {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  create_info.preTransform = support.surface_capabilities.currentTransform;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  create_info.presentMode = present_mode;
  create_info.clipped = VK_TRUE;
  create_info.oldSwapchain = VK_NULL_HANDLE;
  CHECK_VK(FATAL, vkCreateSwapchainKHR(device, &create_info, nullptr, &chain_), "failed to create vk swap chain");

  vkGetSwapchainImagesKHR(device, chain_, &image_count, nullptr);
  images_.resize(image_count);
  vkGetSwapchainImagesKHR(device, chain_, &image_count, &images_[0]);

  format_ = surface_format.format;
  extent_ = extent;
}

void Renderer::DestroySwapChain(const VkDevice& device, const VkAllocationCallbacks* allocator) {
  DestroyImageViews(device, allocator);
  vkDestroySwapchainKHR(device, chain_, allocator);
}

void Renderer::DestroyFramebuffers(const VkDevice& device, const VkAllocationCallbacks* allocator) {
  for (const auto& framebuffer : framebuffers_) {
    vkDestroyFramebuffer(device, framebuffer, allocator);
  }
}

void Renderer::DestroyImageViews(const VkDevice& device, const VkAllocationCallbacks* allocator) {
  for (const auto& view : views_) {
    vkDestroyImageView(device, view, allocator);
  }
}

void Renderer::DestroyRenderPass(Driver* driver) {
  ASSERT(driver);
  vkDestroyRenderPass(driver->GetDevice(), render_pass_, driver->GetAllocator());
}

void Renderer::ReInitSwapChain(Driver* driver) {
  ASSERT(driver);
  const auto window = GetAppWindow();
  ASSERT(window);
  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(window->GetHandle(), &width, &height);
  while (width == 0 && height == 0) {
    glfwGetFramebufferSize(window->GetHandle(), &width, &height);
    glfwWaitEvents();
  }

  driver->WaitDeviceIdle();
  DestroySwapChain(driver->GetDevice());
  DestroyImageViews(driver->GetDevice(), driver->GetAllocator());
  DestroyFramebuffers(driver->GetDevice());

  InitSwapChain(driver->GetPhysicalDevice(), driver->GetDevice(), driver->GetSurface(), driver->GetAllocator());
  InitImageViews(driver->GetDevice());
  InitFramebuffers(driver->GetDevice());
}

void Renderer::DrawFrame(Driver* driver, const Tick& current, const Tick& previous) {
  ASSERT(driver);
  Publish<PreFrameEvent>();
  vkWaitForFences(driver->GetDevice(), 1, &GetCurrentFence(), VK_TRUE, UINT64_MAX);
  uint32_t image_index = 0;
  {
    const auto result = vkAcquireNextImageKHR(driver->GetDevice(), Renderer::GetSwapChain(), UINT64_MAX,
                                              GetCurrentAvailableSemaphore(), VK_NULL_HANDLE, &image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      Renderer::ReInitSwapChain(driver);
      return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      LOG(FATAL) << "failed to acquire swap chain image:" << string_VkResult(result);
    }
  }
  vkResetFences(driver->GetDevice(), 1, &GetCurrentFence());

  Renderer::ResetCommandBuffer(GetCurrentFrame());
  Renderer::RecordCommandBuffer(GetCurrentFrame(), image_index);

  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore wait_semaphores[] = {GetCurrentAvailableSemaphore()};
  VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_semaphores;
  submit_info.pWaitDstStageMask = wait_stages;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &Renderer::GetCommandBuffer(GetCurrentFrame());

  VkSemaphore signal_semaphores[] = {GetCurrentFinishedSemaphore()};
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signal_semaphores;

  const auto& graphics_queue = driver->GetGraphicsQueue();
  CHECK_VK(FATAL, vkQueueSubmit(graphics_queue, 1, &submit_info, GetCurrentFence()), "failed to submit to vk graphics queue");

  VkPresentInfoKHR present_info{};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signal_semaphores;

  VkSwapchainKHR swapchains[] = {Renderer::GetSwapChain()};
  present_info.pSwapchains = swapchains;
  present_info.swapchainCount = 1;
  present_info.pImageIndices = &image_index;

  const auto& present_queue = driver->GetPresentQueue();
  {
    const auto result = vkQueuePresentKHR(present_queue, &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || resized_) {
      resized_ = false;
      Renderer::ReInitSwapChain(driver);
    }
  }

  current_frame_ = ((GetCurrentFrame() + 1) % MAX_NUMBER_OF_FRAMES_IN_FLIGHT);
  Publish<PostFrameEvent>();
}

#define LUA_RENDERER_F(Name) LUA_F(renderer_##Name)

LUA_RENDERER_F(onEvent) {
  OnRendererEvent().subscribe(CreateSubscriber<RendererEvent>(L));
  return 0;
}

#define DEFINE_ON_EVENT_FUNC(Name)                                 \
  LUA_RENDERER_F(on##Name##Event) {                                \
    On##Name##Event().subscribe(CreateSubscriber<Name##Event>(L)); \
    return 0;                                                      \
  }
FOR_EACH_RENDERER_EVENT(DEFINE_ON_EVENT_FUNC);
#undef DEFINE_ON_EVENT_FUNC

#undef LUA_RENDERER_F

// clang-format off
// NOLINTNEXTLINE
static const struct luaL_Reg kRendererLib[] = {
#define LUA_RENDERER_F(Name) \
  { .name = #Name, .func = &lua_renderer_##Name }

  LUA_RENDERER_F(onEvent),
#define DEFINE_ON_EVENT(Name) \
  LUA_RENDERER_F(on##Name##Event),
FOR_EACH_RENDERER_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
#undef LUA_RENDERER_F
};
// clang-format on

void Renderer::InitLua(lua_State* L) {
  ASSERT(L);
  DLOG(INFO) << "initializing lua bindings....";
  lua_newtable(L);
  luaL_setfuncs(L, kRendererLib, 0);
  lua_setglobal(L, "Renderer");
}

void Renderer::Init() {
  OnDriverInitializedEvent().subscribe([](DriverInitializedEvent* event) {
    const auto driver = event->GetDriver();
    // swapchain
    InitSwapChain(driver->GetPhysicalDevice(), driver->GetDevice(), driver->GetSurface(), driver->GetAllocator());
    InitImageViews(driver->GetDevice());
    InitRenderPass(driver->GetDevice());
    InitFramebuffers(driver->GetDevice());
    // pipeline
    InitPipeline(driver);
    InitPipelineCache(driver);
    // command pool
    InitCommandPool(driver->GetPhysicalDevice(), driver->GetDevice(), driver->GetSurface(), driver->GetAllocator());
    InitCommandBuffers(driver->GetDevice());
    InitBuffers();
    // renderer
    InitSyncObjects(driver);
    InitResizeListener();
  });
  engine::OnTerminatingEvent().subscribe([](engine::TerminatingEvent* event) {
    Renderer::Destroy();
  });
}

void Renderer::Destroy() {
  DLOG(INFO) << "destroying vk renderer....";
  const auto driver = Driver::Get();
  ASSERT(driver);
  vkDestroyCommandPool(driver->GetDevice(), command_pool_, driver->GetAllocator());
  DestroySwapChain(driver->GetDevice(), driver->GetAllocator());
  DestroyPipeline(driver);
  DestroyPipelineLayout(driver);
  DestroyPipelineCache(driver);
  ASSERT(index_buffer_);
  index_buffer_->Destroy();
  ASSERT(vertex_buffer_);
  vertex_buffer_->Destroy();
  DestroyFramebuffers(driver->GetDevice(), driver->GetAllocator());
  DestroyRenderPass(driver);
  DestroySyncObjects(driver);
  Publish<RendererDestroyedEvent>();
}
}  // namespace prt