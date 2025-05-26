#ifndef PRT_MAPPED_BUFFER_SCOPE_H
#define PRT_MAPPED_BUFFER_SCOPE_H

#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/vk.h"

namespace prt::vk {

class MappedBufferScope {
 private:
  Buffer* buffer_;
  void* mapped_memory_ = nullptr;

 public:
  MappedBufferScope(Buffer* buffer, const VkDeviceSize size = VK_WHOLE_SIZE, const VkDeviceSize offset = 0,
                    const VkMemoryMapFlags flags = 0);
  ~MappedBufferScope();

  auto GetBuffer() const -> Buffer* {
    return buffer_;
  }

  auto GetMappedMemory() const -> void* {
    return mapped_memory_;
  }

  auto IsMapped() const -> bool {
    return GetMappedMemory() != nullptr;
  }

  void Flush(const VkDeviceSize num_bytes = VK_WHOLE_SIZE, const VkDeviceSize offset = 0);

  operator void*() {
    return mapped_memory_;
  }
};

#define ASSERT_MAPPED(x) ASSERT((x).IsMapped())
}  // namespace prt::vk

#endif  // PRT_MAPPED_BUFFER_SCOPE_H
