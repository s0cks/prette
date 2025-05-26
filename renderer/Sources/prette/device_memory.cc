#include "prette/device_memory.h"

#include "prette/common.h"
#include "prette/device.h"  // IWYU pragma: keep
#include "prette/gfx.h"
#include "prette/to_string.h"
#include "prette/vk.h"

namespace prt::vk {
DeviceMemory::DeviceMemory(const VkMemoryAllocateInfo& alloc_info) :
  HandleTemplate<VkDeviceMemory>() {
  const auto driver = Driver::Get();
  const vk::Result status = vkAllocateMemory(*driver->GetDevice(), &alloc_info, driver->GetAllocator(), handle_ptr());
  LOG_IF(FATAL, !status) << "failed to allocate VkDeviceMemory: " << status;
}

DeviceMemory::~DeviceMemory() {
  const auto driver = Driver::Get();
  vkFreeMemory(*driver->GetDevice(), handle_ref(), driver->GetAllocator());
}

auto DeviceMemory::ToString() const -> std::string {
  return ToStringHelper<DeviceMemory>{};
}
}  // namespace prt::vk