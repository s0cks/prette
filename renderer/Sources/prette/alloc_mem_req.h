#ifndef PRT_ALLOC_MEM_REQ_H
#define PRT_ALLOC_MEM_REQ_H

#include <cstdint>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/vk.h"

namespace prt::vk {
class AllocMemoryRequest {
 private:
  VkMemoryAllocateInfo info_{};

  inline auto info_ptr() -> VkMemoryAllocateInfo* {
    return &info_;
  }

 public:
  AllocMemoryRequest(const VkDeviceSize alloc_size, const uint32_t type);
  AllocMemoryRequest(const VkMemoryRequirements requirements, const VkMemoryPropertyFlags flags);
  AllocMemoryRequest(const VkImage image, const VkMemoryPropertyFlags flags);
  AllocMemoryRequest(const VkBuffer buffer, const VkMemoryPropertyFlags flags);
  ~AllocMemoryRequest() = default;

  auto info() const -> const VkMemoryAllocateInfo& {
    return info_;
  }

  auto WithNext(const void* rhs) -> AllocMemoryRequest& {
    ASSERT(rhs);
    info_ptr()->pNext = rhs;
    return *this;
  }

  auto WithAllocationSize(const VkDeviceSize rhs) -> AllocMemoryRequest& {
    ASSERT(rhs > 0);
    info_ptr()->allocationSize = rhs;
    return *this;
  }

  auto WithMemoryTypeIndex(const uint32_t rhs) -> AllocMemoryRequest& {
    info_ptr()->memoryTypeIndex = rhs;
    return *this;
  }

  void Allocate(VkDeviceMemory* result);

  void operator()(VkDeviceMemory* result) {
    return Allocate(result);
  }
};
}  // namespace prt::vk

#endif  // PRT_ALLOC_MEM_REQ_H
