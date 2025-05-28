
#include "prette/vk_cmd_buffers.h"

#include <fmt/format.h>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/command_pool.h"  // IWYU pragma: keep
#include "prette/common.h"
#include "prette/device.h"
#include "prette/gfx_vk.h"
#include "prette/platform.h"
#include "prette/swapchain/swapchain.h"
#include "prette/vk.h"
#include "prette/vk_fence.h"

namespace prt::vk {
void SingleUseCommandBuffer::Start(const VkCommandBuffer& buffer) {
  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  CHECK_VK(FATAL, vkBeginCommandBuffer(buffer, &begin_info), "failed to begin vk command buffer");
}

void SingleUseCommandBuffer::Finish(const VkCommandBuffer& buffer) {
  CHECK_VK(FATAL, vkEndCommandBuffer(buffer), "failed to end single use vk command buffer");
}

void SingleUseCommandBuffer::Submit(const VkDevice& device, const VkQueue& queue, const VkCommandBuffer& buffer,
                                    const VkAllocationCallbacks* allocator) {
  SingleUseFence fence{};
  vk::Submit(queue, &buffer, 1, (VkFence)fence);
  fence.Wait();
}

static inline void DestroyCommandBuffer(const VkDevice& device, const VkCommandPool& command_pool,
                                        const VkCommandBuffer& buffer) {
  vkFreeCommandBuffers(device, command_pool, 1, &buffer);
}

SingleUseCommandBuffer::SingleUseCommandBuffer() {
  const auto driver = Driver::Get();
  ASSERT(driver);
  AllocateCommandBuffers alloc(1);
  alloc.Allocate(&buffer_);
  Start(buffer_);
}

SingleUseCommandBuffer::~SingleUseCommandBuffer() {
  const auto driver = Driver::Get();
  ASSERT(driver);
  CHECK_VK(FATAL, vkEndCommandBuffer(buffer_), "failed to end vk command buffer");
  const auto& device = driver->GetDevice();
  Submit(*driver->GetDevice(), device->GetGraphicsQueue(), buffer_, driver->GetAllocator());
  vk::FreeCommandBuffers(&buffer_, 1);
}

CommandBufferScope::CommandBufferScope(VkCommandBuffer* buffer, const bool reset) :
  buffer_(buffer) {
  begin_info_.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  if (reset)
    CHECK_VK(FATAL, vkResetCommandBuffer(*buffer, 0), "failed to reset vk command buffer");
  CHECK_VK(FATAL, vkBeginCommandBuffer(*buffer, &begin_info_), "failed to begin vk command buffer");
}

CommandBufferScope::~CommandBufferScope() {
  CHECK_VK(FATAL, vkEndCommandBuffer(*buffer_), "failed to end vk command buffer");
  const auto frame = GetSwapchain()->GetCurrentFrame();
  frame->Append(*buffer_);
}

AllocateCommandBuffers::AllocateCommandBuffers(const uint32_t num_buffers) {
  info_ptr()->sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info_ptr()->commandBufferCount = num_buffers;
  info_ptr()->commandPool = *Driver::Get()->GetCommandPool();
  info_ptr()->level = kDefaultLevel;
}

auto AllocateCommandBuffers::IsValid() const -> bool {
  return GetNumberOfBuffers() > 0;
}

auto AllocateCommandBuffers::WithPool(CommandPool* rhs) -> AllocateCommandBuffers& {
  return WithPool(*rhs);
}

auto AllocateCommandBuffers::Allocate(std::vector<VkCommandBuffer>& results) -> bool {
  ASSERT(IsValid());
  const auto driver = Driver::Get();
  results.resize(GetNumberOfBuffers());
  driver->GetCommandPool()->AllocCommandBuffers(info_ptr(), &results[0]);
  return true;
}

auto AllocateCommandBuffers::Allocate(VkCommandBuffer* results) -> bool {
  ASSERT(IsValid());
  const auto driver = Driver::Get();
  driver->GetCommandPool()->AllocCommandBuffers(info_ptr(), results);
  return true;
}

void FreeCommandBuffers(VkCommandBuffer* buffers, const uint64_t num_buffers) {
  const auto driver = Driver::Get();
  driver->GetCommandPool()->FreeCommandBuffers(buffers, num_buffers);
}

void Submit(const VkQueue& queue, const VkCommandBuffer* buffers, const uint64_t num_buffers, const VkFence& fence) {
  VkSubmitInfo submit{};
  submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit.commandBufferCount = num_buffers;
  submit.pCommandBuffers = buffers;
  CHECK_VK(FATAL, vkQueueSubmit(queue, 1, &submit, fence),
           fmt::format("failed to submit {} VkCommandBuffers to queue.", num_buffers));
}
}  // namespace prt::vk