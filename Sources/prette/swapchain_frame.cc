#include "prette/swapchain_frame.h"

#include <vulkan/vulkan_core.h>

#include "prette/swapchain.h"
#include "prette/to_string.h"

namespace prt {
SwapChainFrame::SwapChainFrame(const uint32_t f) :
  frame_(f) {
  const auto driver = Driver::Get();

  VkSemaphoreCreateInfo semaphore_info{};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  CHECK_VK(FATAL, vkCreateSemaphore(driver->GetDevice(), &semaphore_info, driver->GetAllocator(), &available_),
           "failed to create available semaphore");
  CHECK_VK(FATAL, vkCreateSemaphore(driver->GetDevice(), &semaphore_info, driver->GetAllocator(), &finished_),
           "failed to create finished semaphore");

  VkFenceCreateInfo fence_info{};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  CHECK_VK(FATAL, vkCreateFence(driver->GetDevice(), &fence_info, driver->GetAllocator(), &fence_), "failed to create fence");
}

SwapChainFrame::~SwapChainFrame() {
  Driver::Get()->Destroy(finished_, vkDestroySemaphore);
  Driver::Get()->Destroy(available_, vkDestroySemaphore);
  Driver::Get()->Destroy(fence_, vkDestroyFence);
}

auto SwapChainFrame::ToString() const -> std::string {
  ToStringHelper<SwapChainFrame> helper{};
  helper.AddFieldRef("frame", GetFrame());
  helper.AddFieldRef("image", GetImage());
  return helper;
}
}  // namespace prt