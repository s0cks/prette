#include "prette/scene_renderer.h"

#include <fmt/format.h>
#include <vulkan/vulkan_core.h>

#include "prette/camera.h"
#include "prette/common.h"
#include "prette/gfx.h"
#include "prette/gfx_vk.h"
#include "prette/gui.h"
#include "prette/pipeline.h"
#include "prette/render_target.h"
#include "prette/renderer.h"
#include "prette/shader.h"
#include "prette/swapchain.h"

namespace prt {
static VkRenderPass pass_;
static std::array<RenderTarget, MAX_NUMBER_OF_FRAMES_IN_FLIGHT + 1> render_targets_{};
static std::array<VkCommandBuffer, MAX_NUMBER_OF_FRAMES_IN_FLIGHT> command_buffers_{};

static GraphicsPipeline pipeline_;

static VkDescriptorSetLayout descriptor_set_layout_{};
static VkDescriptorPool descriptor_pool_{};
static std::vector<VkDescriptorSet> descriptor_sets_{};

static std::vector<vk::Buffer*> camera_buffers_{};
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
  color_attachment.format = SwapChain::GetFormat();
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

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

  std::array<VkSubpassDependency, 1> dependencies = {
      dependency1,
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

void SceneRenderer::InitCommandBuffers(const Driver* driver) {
  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = driver->GetCommandPool();
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = MAX_NUMBER_OF_FRAMES_IN_FLIGHT;
  CHECK_VK(FATAL, vkAllocateCommandBuffers(driver->GetDevice(), &alloc_info, &command_buffers_[0]),
           "failed to allocate vk command buffers");
}

void SceneRenderer::InitPipeline(const Driver* driver) {
  ASSERT(driver);
  std::vector<VkDynamicState> dynamic_states = {};
  std::vector<VkDescriptorSetLayout> descriptor_set_layouts = {
      descriptor_set_layout_,
  };
  pipeline_ = GraphicsPipeline(driver, "scene", pass_, dynamic_states, SwapChain::GetExtent(), descriptor_set_layouts);
}

void SceneRenderer::InitImages(const Driver* driver, const uint64_t num_images, const VkExtent2D& extent) {
  for (auto idx = 0; idx < num_images; idx++) {
    render_targets_[idx] = RenderTarget(pass_, extent);
  }
}

void SceneRenderer::Draw(const SwapChainFrame& frame, std::vector<VkCommandBuffer>& cmd_buffers) {
  // clang-format off
  static const std::vector<VkClearValue> kClearValues = {
    VkClearValue { .color = { 0.0f, 0.0f, 0.0f, 1.0f }}
  };
  // clang-format on
  CommandBufferScope buffer(command_buffers_.at(frame), true);
  {
    RenderPassScope render_pass(buffer, pass_, render_targets_[frame.image].GetFramebuffer(), kClearValues);
    render_pass.Bind(&pipeline_);
    vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_.GetLayout(), 0, 1, &descriptor_sets_[frame], 0,
                            nullptr);
    VkBuffer vertex_buffers[] = {vertex_buffer_->GetBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(buffer, 0, 1, vertex_buffers, offsets);
    vkCmdBindIndexBuffer(buffer, index_buffer_->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(buffer, indices.size(), 1, 0, 0, 0);
  }
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
  {
    // camera buffer
    for (auto idx = 0; idx < MAX_NUMBER_OF_FRAMES_IN_FLIGHT; idx++) {
      const auto buffer = vk::Buffer::NewUniformBuffer<CameraData>();
      ASSERT(buffer);
      camera_buffers_.push_back(buffer);
    }
  }
}

void SceneRenderer::Destroy(const Driver* driver, const bool is_reinit) {
  ASSERT(driver);
  if (!is_reinit) {
    for (const auto& buffer : camera_buffers_) {
      delete buffer;
    }
    vkDestroyDescriptorSetLayout(driver->GetDevice(), descriptor_set_layout_, driver->GetAllocator());
    vkDestroyDescriptorPool(driver->GetDevice(), descriptor_pool_, driver->GetAllocator());
    pipeline_.Destroy(driver);
    vkDestroyRenderPass(driver->GetDevice(), pass_, driver->GetAllocator());
  }
  if (!is_reinit) {
    ASSERT(vertex_buffer_);
    vertex_buffer_->Destroy();
    ASSERT(index_buffer_);
    index_buffer_->Destroy();
    driver->ReleaseCommandBuffers(command_buffers_);
    for (const auto& target : render_targets_) {
      target.Destroy();
    }
  }
}

auto SceneRenderer::GetCameraBuffer(const uint64_t idx) -> vk::Buffer* {
  return camera_buffers_[idx];
}

auto SceneRenderer::GetRenderPass() -> VkRenderPass const& {
  return pass_;
}

auto SceneRenderer::GetImageView(const uint64_t idx) -> VkImageView const& {
  return render_targets_[idx].GetView();
}

void SceneRenderer::InitDescriptorSetLayout(const Driver* driver) {
  ASSERT(driver);
  VkDescriptorSetLayoutBinding camera_binding{};
  camera_binding.binding = 0;
  camera_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  camera_binding.descriptorCount = 1;
  camera_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  camera_binding.pImmutableSamplers = nullptr;

  std::array<VkDescriptorSetLayoutBinding, 1> bindings = {
      camera_binding,
  };

  VkDescriptorSetLayoutCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  create_info.bindingCount = bindings.size();
  create_info.pBindings = bindings.data();

  CHECK_VK(FATAL, vkCreateDescriptorSetLayout(driver->GetDevice(), &create_info, driver->GetAllocator(), &descriptor_set_layout_),
           "failed to create vk descriptor set layout");
}

void SceneRenderer::InitDescriptorPool(const Driver* driver) {
  ASSERT(driver);
  std::vector<VkDescriptorPoolSize> pool_sizes{
      {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = MAX_NUMBER_OF_FRAMES_IN_FLIGHT},
  };
  VkDescriptorPoolCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  create_info.poolSizeCount = pool_sizes.size();
  create_info.pPoolSizes = pool_sizes.data();
  create_info.maxSets = MAX_NUMBER_OF_FRAMES_IN_FLIGHT;
  CHECK_VK(FATAL, vkCreateDescriptorPool(driver->GetDevice(), &create_info, driver->GetAllocator(), &descriptor_pool_),
           "failed to create vk descriptor pool");
}

void SceneRenderer::InitDescriptorSets(const Driver* driver) {
  ASSERT(driver);
  std::vector<VkDescriptorSetLayout> layouts(MAX_NUMBER_OF_FRAMES_IN_FLIGHT, descriptor_set_layout_);
  VkDescriptorSetAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = descriptor_pool_;
  alloc_info.descriptorSetCount = layouts.size();
  alloc_info.pSetLayouts = layouts.data();
  descriptor_sets_.resize(layouts.size());
  CHECK_VK(FATAL, vkAllocateDescriptorSets(driver->GetDevice(), &alloc_info, descriptor_sets_.data()),
           "failed to allocate vk descriptor sets");

  for (auto idx = 0; idx < MAX_NUMBER_OF_FRAMES_IN_FLIGHT; idx++) {
    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = camera_buffers_[idx]->GetBuffer();
    buffer_info.offset = 0;
    buffer_info.range = sizeof(CameraData);

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descriptor_sets_[idx];
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.descriptorCount = 1;
    write.pBufferInfo = &buffer_info;
    write.pImageInfo = nullptr;
    write.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(driver->GetDevice(), 1, &write, 0, nullptr);
  }
}

void SceneRenderer::Init() {
  OnSwapChainInitEvent().subscribe([](SwapChainInitEvent* event) {
    try {
      ASSERT(event);
      const auto driver = Driver::Get();
      ASSERT(driver);
      if (!event->IsReinit()) {
        InitRenderPass(driver);
        InitDescriptorSetLayout(driver);
        InitPipeline(driver);
        InitCommandBuffers(driver);
      }
      InitImages(driver, SwapChain::GetNumberOfImages(), SwapChain::GetExtent());
      if (!event->IsReinit()) {
        InitBuffers();
        InitDescriptorPool(driver);
        InitDescriptorSets(driver);
      }
    } catch (const std::exception& exc) {
      LOG(FATAL) << "exception: " << exc.what();
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