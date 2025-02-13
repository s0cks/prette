#include "prette/gui_renderer.h"

#include <imgui_impl_vulkan.h>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/gfx.h"
#include "prette/gfx_vk.h"
#include "prette/renderer.h"
#include "prette/scene_renderer.h"
#include "prette/shader.h"

namespace prt {
static VkPipeline pipeline_{};
static VkPipelineLayout pipeline_layout_{};
static VkPipelineCache pipeline_cache_{};

static VkRenderPass pass_{};
static std::vector<VkFramebuffer> framebuffers_{};

static VkCommandPool command_pool_{};
static std::array<VkCommandBuffer, MAX_NUMBER_OF_FRAMES_IN_FLIGHT> command_buffers_{};

static VkSampler sampler_{};
static VkDescriptorPool descriptor_pool_{};
static std::vector<VkDescriptorSet> descriptors_{};

static GuiRendererEventSubject events_{};

auto OnGuiRendererEvent() -> GuiRendererEventObservable {
  return events_.get_observable();
}

static inline void PublishEvent(GuiRendererEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

template <class E, typename... Args>
static inline void Publish(Args... args) {
  E event(args...);
  return PublishEvent(&event);
}

void GuiRenderer::InitPipeline(const Driver* driver) {
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

void GuiRenderer::InitCommandPool(const Driver* driver) {
  const auto indices = FindQueueFamilies(driver->GetPhysicalDevice(), driver->GetSurface());
  VkCommandPoolCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  create_info.queueFamilyIndex = indices.GetGraphicsFamily();
  CHECK_VK(FATAL, vkCreateCommandPool(driver->GetDevice(), &create_info, driver->GetAllocator(), &command_pool_),
           "failed to create vk command pool");
}

void GuiRenderer::InitCommandBuffers(const Driver* driver) {
  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = command_pool_;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = command_buffers_.size();
  CHECK_VK(FATAL, vkAllocateCommandBuffers(driver->GetDevice(), &alloc_info, command_buffers_.data()),
           "failed to allocate vk command buffers");
}

void GuiRenderer::InitRenderPass(const Driver* driver) {
  VkAttachmentDescription color_attachment{};
  color_attachment.format = Renderer::GetFormat();
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
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
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  create_info.attachmentCount = 1;
  create_info.pAttachments = &color_attachment;
  create_info.subpassCount = 1;
  create_info.pSubpasses = &subpass;
  create_info.dependencyCount = 1;
  create_info.pDependencies = &dependency;

  CHECK_VK(FATAL, vkCreateRenderPass(driver->GetDevice(), &create_info, nullptr, &pass_), "failed to create vk render pass");
}

void GuiRenderer::InitFramebuffers(const Driver* driver) {
  ASSERT(driver);
  vk::InitFramebuffers(driver, pass_, Renderer::GetImageViews(), Renderer::GetExtent(), framebuffers_);
}

void GuiRenderer::Destroy(const Driver* driver, const bool is_reinit) {
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
    vkFreeCommandBuffers(driver->GetDevice(), command_pool_, command_buffers_.size(), command_buffers_.data());
    vkDestroyCommandPool(driver->GetDevice(), command_pool_, driver->GetAllocator());
  }
}

auto GuiRenderer::GetPass() -> VkRenderPass const& {
  return pass_;
}

auto GuiRenderer::GetSceneDescriptor(const uint32_t frame) -> VkDescriptorSet const& {
  return descriptors_[frame];
}

void GuiRenderer::InitTextureSampler(const Driver* driver) {
  ASSERT(driver);
  VkSamplerCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  create_info.magFilter = VK_FILTER_LINEAR;
  create_info.minFilter = VK_FILTER_LINEAR;
  create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  create_info.anisotropyEnable = VK_FALSE;
  create_info.maxAnisotropy = 1.0f;
  create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  create_info.unnormalizedCoordinates = VK_FALSE;
  create_info.compareEnable = VK_FALSE;
  create_info.compareOp = VK_COMPARE_OP_ALWAYS;
  create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  create_info.mipLodBias = 0.0f;
  create_info.minLod = 0.0f;
  create_info.maxLod = 0.0f;
  CHECK_VK(FATAL, vkCreateSampler(driver->GetDevice(), &create_info, nullptr, &sampler_), "failed to create vk sampler");
}

static auto GetDescriptorPool() -> VkDescriptorPool const& {
  return descriptor_pool_;
}

void GuiRenderer::InitDescriptorPool(const Driver* driver) {
  VkDescriptorPoolSize descriptor_pool_sizes_[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                                   {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                                   {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                                   {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                                   {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                                   {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                                   {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                                   {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                                   {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                                   {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                                   {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

  VkDescriptorPoolCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  create_info.maxSets = 1000;
  create_info.poolSizeCount = std::size(descriptor_pool_sizes_);
  create_info.pPoolSizes = descriptor_pool_sizes_;

  CHECK_VK(FATAL, vkCreateDescriptorPool(driver->GetDevice(), &create_info, driver->GetAllocator(), &descriptor_pool_),
           "failed to create vk descriptor pool");
}

void GuiRenderer::InitSceneDescriptors(const Driver* driver) {
  ASSERT(driver);
  descriptors_.resize(MAX_NUMBER_OF_FRAMES_IN_FLIGHT);
  for (auto idx = 0; idx < MAX_NUMBER_OF_FRAMES_IN_FLIGHT; idx++) {
    descriptors_[idx] =
        ImGui_ImplVulkan_AddTexture(sampler_, Renderer::GetImageView(idx), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  }
}

auto GuiRenderer::GetDescriptorPool() -> VkDescriptorPool const& {
  return descriptor_pool_;
}

void GuiRenderer::Init() {
  OnSwapChainInitEvent().subscribe([](SwapChainInitEvent* event) {
    ASSERT(event);
    const auto driver = Driver::Get();
    ASSERT(driver);
    if (!event->IsReinit()) {
      DLOG(INFO) << "iniializing GuiRenderer....";
      InitRenderPass(driver);
    }
    InitFramebuffers(driver);
    if (!event->IsReinit()) {
      InitPipeline(driver);
      InitCommandPool(driver);
      InitCommandBuffers(driver);
      InitTextureSampler(driver);
      InitDescriptorPool(driver);
      ImGui_ImplVulkan_InitInfo info{};
      info.Instance = driver->GetInstance();
      info.PhysicalDevice = driver->GetPhysicalDevice();
      info.Device = driver->GetDevice();
      info.Queue = driver->GetGraphicsQueue();
      info.DescriptorPool = GetDescriptorPool();
      info.ImageCount = 2;
      info.MinImageCount = 2;
      info.RenderPass = GetPass();
      info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
      ImGui_ImplVulkan_Init(&info);

      ImGui_ImplVulkan_CreateFontsTexture();
      ImGui_ImplVulkan_DestroyFontsTexture();
    }
  });
  OnRendererInitEvent().subscribe([](RendererInitEvent* event) {
    ASSERT(event);
    const auto driver = Driver::Get();
    ASSERT(driver);
    InitSceneDescriptors(driver);
  });
  OnSwapChainDestroyedEvent().subscribe([](SwapChainDestroyedEvent* event) {
    ASSERT(event);
    const auto driver = Driver::Get();
    ASSERT(driver);
    Destroy(driver, event->IsReinit());
  });
}

void GuiRenderer::Draw(const uint32_t bidx, const uint32_t image_index, std::vector<VkCommandBuffer>& cmd_buffers) {
  // do nothing
}
}  // namespace prt