#include "prette/command_pool.h"

#include <vulkan/vulkan_core.h>

#include "prette/gfx.h"
#include "prette/pipeline.h"
#include "prette/swap_chain.h"

namespace prt {
static VkCommandPool command_pool_{};
static std::array<VkCommandBuffer, MAX_NUMBER_OF_FRAMES_IN_FLIGHT> command_buffers_{};

auto CommandPool::GetCommandPool() -> const VkCommandPool& {
  return command_pool_;
}

void CommandPool::InitCommandPool(const VkPhysicalDevice& physical_device, const VkDevice& device, const VkSurfaceKHR& surface,
                                  const VkAllocationCallbacks* allocator) {
  const auto indices = FindQueueFamilies(physical_device, surface);
  VkCommandPoolCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  create_info.queueFamilyIndex = indices.GetGraphicsFamily();
  CHECK_VK(FATAL, vkCreateCommandPool(device, &create_info, allocator, &command_pool_), "failed to create vk command pool");
}

void CommandPool::Init(Driver* driver) {
  ASSERT(driver);
  InitCommandPool(driver->GetPhysicalDevice(), driver->GetDevice(), driver->GetSurface(), driver->GetAllocator());
  InitCommandBuffers(driver->GetDevice());
}

void CommandPool::InitCommandBuffers(const VkDevice& device) {
  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = command_pool_;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = command_buffers_.size();
  CHECK_VK(FATAL, vkAllocateCommandBuffers(device, &alloc_info, &command_buffers_[0]), "failed to allocate vk command buffers");
}

auto CommandPool::GetCommandBuffer(const uint32_t buffer_index) -> const VkCommandBuffer& {
  return command_buffers_.at(buffer_index);
}

void CommandPool::ResetCommandBuffer(const uint32_t buffer_index, const VkCommandBufferResetFlagBits flags) {
  CHECK_VK(FATAL, vkResetCommandBuffer(GetCommandBuffer(buffer_index), flags), "failed to reset vk command buffer");
}

void CommandPool::RecordCommandBuffer(const uint32_t buffer_index, const uint32_t image_index) {
  auto& buffer = GetCommandBuffer(buffer_index);
  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  CHECK_VK(FATAL, vkBeginCommandBuffer(buffer, &begin_info), "failed to begin command buffer recording");
  {
    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = SwapChain::GetRenderPass();
    render_pass_info.framebuffer = SwapChain::GetFramebuffer(image_index);
    render_pass_info.renderArea.offset = {.x = 0, .y = 0};
    render_pass_info.renderArea.extent = SwapChain::GetExtent();

    VkClearValue clear_color{{0.0f, 0.0f, 0.0f, 1.0f}};
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass(buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
    {
      vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline::GetPipeline());
      VkViewport viewport{};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = (float)SwapChain::GetExtent().width;
      viewport.height = (float)SwapChain::GetExtent().height;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(buffer, 0, 1, &viewport);

      VkRect2D scissor{};
      scissor.offset = {.x = 0, .y = 0};
      scissor.extent = SwapChain::GetExtent();
      vkCmdSetScissor(buffer, 0, 1, &scissor);

      VkBuffer vertex_buffers[] = {Pipeline::GetVertexBuffer()};
      VkDeviceSize offsets[] = {0};
      vkCmdBindVertexBuffers(buffer, 0, 1, vertex_buffers, offsets);

      vkCmdBindIndexBuffer(buffer, Pipeline::GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);

      vkCmdDrawIndexed(buffer, Pipeline::GetNumberOfIndices(), 1, 0, 0, 0);
    }
    vkCmdEndRenderPass(buffer);
  }
  CHECK_VK(FATAL, vkEndCommandBuffer(buffer), "failed to end command buffer recording");
}

void CommandPool::Shutdown(Driver* driver) {
  ASSERT(driver);
  vkDestroyCommandPool(driver->GetDevice(), command_pool_, driver->GetAllocator());
}
}  // namespace prt