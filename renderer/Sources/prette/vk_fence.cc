#include "prette/vk_fence.h"

#include <string>

#include "prette/device.h"  // IWYU pragma: keep
#include "prette/gfx.h"
#include "prette/platform.h"
#include "prette/to_string.h"
#include "prette/vk.h"

namespace prt::vk {
SingleUseFence::SingleUseFence(const VkFenceCreateFlags flags) {
  const auto driver = Driver::Get();
  VkFenceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  create_info.flags = flags;
  CHECK_VK(FATAL, vkCreateFence(*driver->GetDevice(), &create_info, driver->GetAllocator(), handle_ptr()),
           "failed to create single use VkFence");
}

SingleUseFence::~SingleUseFence() {
  const auto driver = Driver::Get();
  vkDestroyFence(*driver->GetDevice(), handle_ref(), driver->GetAllocator());
}

auto SingleUseFence::ToString() const -> std::string {
  return ToStringHelper<SingleUseFence>{};
}

void SingleUseFence::Wait(const uint64_t timeout, const bool waitAll) {
  const auto driver = Driver::Get();
  CHECK_VK(FATAL, vkWaitForFences(*driver->GetDevice(), 1, handle_ptr(), waitAll, timeout),
           "failed to wait for VkFence");
}
}  // namespace prt::vk