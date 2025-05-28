#include "prette/command_pool.h"

#include <string>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/device.h"  // IWYU pragma: keep
#include "prette/gfx.h"
#include "prette/to_string.h"
#include "prette/vk.h"

namespace prt::vk {
CommandPoolBuilder::CommandPoolBuilder() {
  info_ptr()->sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
}

auto CommandPoolBuilder::Build() -> CommandPool* {
  ASSERT(IsValid());
  return new CommandPool(info_ptr());
}

auto CommandPoolBuilder::IsValid() const -> bool {
  NOT_IMPLEMENTED(ERROR);
  return true;
}

CommandPool::CommandPool(const VkCommandPoolCreateInfo* create_info) {
  ASSERT(create_info);
  const auto driver = Driver::Get();
  driver->CreateCommandPool(create_info, handle_ptr());
}

CommandPool::~CommandPool() {
  const auto driver = Driver::Get();
  driver->DestroyCommandPool(handle_ref());
}

void CommandPool::AllocCommandBuffers(const VkCommandBufferAllocateInfo* alloc_info, VkCommandBuffer* results) {
  const auto driver = Driver::Get();
  const vk::Result status = vkAllocateCommandBuffers(*driver->GetDevice(), alloc_info, results);
  LOG_IF(FATAL, !status) << "failed to allocate " << alloc_info->commandBufferCount << " VkCommandBuffers: " << status;
}

void CommandPool::FreeCommandBuffers(VkCommandBuffer* buffers, const uint64_t num_buffers) {
  ASSERT(buffers && num_buffers > 0);
  const auto driver = Driver::Get();
  vkFreeCommandBuffers(*driver->GetDevice(), GetHandle(), num_buffers, buffers);
}

auto CommandPool::ToString() const -> std::string {
  return ToStringHelper<std::string>{};
}
}  // namespace prt::vk