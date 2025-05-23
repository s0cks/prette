#ifndef PRT_DEVICE_MEMORY_H
#define PRT_DEVICE_MEMORY_H

#include <string>

#include "prette/vk.h"

namespace prt::vk {
class DeviceMemory : public HandleTemplate<VkDeviceMemory> {
 public:
  explicit DeviceMemory(const VkMemoryAllocateInfo& alloc_ino);
  ~DeviceMemory() override;
  auto ToString() const -> std::string override;
  operator VkDeviceMemory() const {
    return GetHandle();
  }
};
}  // namespace prt::vk

#endif  // PRT_DEVICE_MEMORY_H
