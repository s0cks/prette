#ifndef PRT_VK_FENCE_H
#define PRT_VK_FENCE_H

#include <string>

#include "prette/platform.h"
#include "prette/vk.h"

namespace prt::vk {
#ifndef VK_DEFAULT_FENCE_TIMEOUT
#define VK_DEFAULT_FENCE_TIMEOUT UINT64_MAX
#endif  // VK_DEFAULT_FENCE_TIMEOUT

class SingleUseFence : public HandleTemplate<VkFence> {
 public:
  explicit SingleUseFence(const VkFenceCreateFlags flags = 0);
  ~SingleUseFence();

  auto ToString() const -> std::string override;
  void Wait(const uint64_t timeout = VK_DEFAULT_FENCE_TIMEOUT, const bool waitAll = true);

  operator VkFence() const {
    return GetHandle();
  }
};
}  // namespace prt::vk

#endif  // PRT_VK_FENCE_H
