#include "prette/device.h"

#include <string>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/gfx.h"
#include "prette/gfx_vk.h"
#include "prette/surface.h"
#include "prette/to_string.h"
#include "prette/vk.h"
#include "prette/vk_physical_device.h"

namespace prt::vk {
Device::Device(VkDeviceCreateInfo* create_info) {
  const auto driver = Driver::Get();
  const auto& queue_families = driver->GetPhysicalDevice()->GetQueueFamilies();
  driver->CreateDevice(create_info, handle_ptr());
  queue_families.GetGraphicsQueue(handle_ref(), graphics_queue_);
  ASSERT(HasGraphicsQueue());
  queue_families.GetPresentQueue(handle_ref(), present_queue_);
  ASSERT(HasPresentQueue());
}

Device::~Device() {
  if (IsInitialized()) {
    const auto driver = Driver::Get();
    vkDestroyDevice(handle_ref(), driver->GetAllocator());
  }
}

auto Device::ToString() const -> std::string {
  return ToStringHelper<Device>{};
}

auto Device::MapDeviceMemory(const VkDeviceMemory& device_memory, const VkDeviceSize size, void** result,
                             const VkDeviceSize offset, const VkMemoryMapFlags flags) -> Result {
  return vkMapMemory(handle_ref(), device_memory, offset, size, flags, result);
}

void Device::UnmapDeviceMemory(const VkDeviceMemory& device_memory) {
  if (device_memory == VK_NULL_HANDLE) {
    DLOG(WARNING) << "trying to unmap null handle device memory.";
    return;
  }
  return vkUnmapMemory(handle_ref(), device_memory);
}
}  // namespace prt::vk