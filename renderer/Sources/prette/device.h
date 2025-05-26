#ifndef PRT_DEVICE_H
#define PRT_DEVICE_H

#include <string>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/gfx_vk.h"
#include "prette/vk.h"

namespace prt::vk {
class Device : public HandleTemplate<VkDevice> {
  friend class prt::Driver;
  DEFINE_DEFAULT_COPYABLE_TYPE(Device);

 private:
  float priority_ = kDefaultDevicePriority;
  VkQueue graphics_queue_ = VK_NULL_HANDLE;
  VkQueue present_queue_ = VK_NULL_HANDLE;

 public:
  Device() = default;
  explicit Device(VkDeviceCreateInfo* create_info);
  ~Device();

  auto GetGraphicsQueue() const -> VkQueue {
    return graphics_queue_;
  }

  inline auto HasGraphicsQueue() const -> bool {
    return GetGraphicsQueue() != VK_NULL_HANDLE;
  }

  auto GetPresentQueue() const -> VkQueue {
    return present_queue_;
  }

  inline auto HasPresentQueue() const -> bool {
    return GetPresentQueue() != VK_NULL_HANDLE;
  }

  auto MapDeviceMemory(const VkDeviceMemory& device_memory, const VkDeviceSize size, void** result,
                       const VkDeviceSize offset = 0, const VkMemoryMapFlags flags = 0) -> Result;
  void UnmapDeviceMemory(const VkDeviceMemory& device_memory);
  auto ToString() const -> std::string override;
  operator VkDevice() const {
    return GetHandle();
  }

 public:
  static void Init();
};
}  // namespace prt::vk

#endif  // PRT_DEVICE_H
