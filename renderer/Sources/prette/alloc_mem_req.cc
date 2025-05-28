#include "prette/alloc_mem_req.h"

#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/device.h"  // IWYU pragma: keep
#include "prette/gfx.h"
#include "prette/vk.h"
#include "prette/vk_physical_device.h"

namespace prt::vk {
AllocMemoryRequest::AllocMemoryRequest(const VkDeviceSize alloc_size, const uint32_t type) {
  info_ptr()->sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  info_ptr()->allocationSize = alloc_size;
  info_ptr()->memoryTypeIndex = type;
}

AllocMemoryRequest::AllocMemoryRequest(const VkMemoryRequirements reqs, const VkMemoryPropertyFlags props) {
  const auto driver = Driver::Get();
  const auto mem_type = driver->GetPhysicalDevice()->FindMemoryType(reqs.memoryTypeBits, props);
  LOG_IF(FATAL, !mem_type) << "failed to find valid memory type.";
  info_ptr()->sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  info_ptr()->allocationSize = reqs.size;
  info_ptr()->memoryTypeIndex = (*mem_type);
}

AllocMemoryRequest::AllocMemoryRequest(const VkBuffer buffer, const VkMemoryPropertyFlags mem_props) {
  const auto driver = Driver::Get();
  VkMemoryRequirements mem_reqs{};
  vkGetBufferMemoryRequirements(*driver->GetDevice(), buffer, &mem_reqs);
  const auto mem_type = driver->GetPhysicalDevice()->FindMemoryType(mem_reqs.memoryTypeBits, mem_props);
  LOG_IF(FATAL, !mem_type) << "failed to find valid memory type.";
  info_ptr()->sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  info_ptr()->allocationSize = mem_reqs.size;
  info_ptr()->memoryTypeIndex = (*mem_type);
}

AllocMemoryRequest::AllocMemoryRequest(const VkImage image, const VkMemoryPropertyFlags props) {
  const auto driver = Driver::Get();
  VkMemoryRequirements reqs{};
  vkGetImageMemoryRequirements(*driver->GetDevice(), image, &reqs);
  const auto mem_type = driver->GetPhysicalDevice()->FindMemoryType(reqs.memoryTypeBits, props);
  LOG_IF(FATAL, !mem_type) << "failed to find valid memory type.";
  info_ptr()->sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  info_ptr()->allocationSize = reqs.size;
  info_ptr()->memoryTypeIndex = (*mem_type);
}

void AllocMemoryRequest::Allocate(VkDeviceMemory* memory) {
  const auto driver = Driver::Get();
  CHECK_VK(FATAL, vkAllocateMemory(*driver->GetDevice(), &info(), driver->GetAllocator(), memory),
           "failed to allocate VkDeviceMemory");
}
}  // namespace prt::vk