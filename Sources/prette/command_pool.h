#ifndef PRT_COMMAND_POOL_H
#define PRT_COMMAND_POOL_H

#include <vulkan/vulkan_core.h>

#include "prette/gfx.h"

namespace prt {
class CommandPool {
 public:
  static void Init(Driver* driver);
  static void Shutdown(Driver* driver);
  static auto GetCommandPool() -> const VkCommandPool&;
  static auto GetCommandBuffer(const uint32_t buffer_index) -> const VkCommandBuffer&;
  static void RecordCommandBuffer(const uint32_t buffer_index, const uint32_t frame_index);
  static void ResetCommandBuffer(const uint32_t buffer_index,
                                 const VkCommandBufferResetFlagBits flags = static_cast<VkCommandBufferResetFlagBits>(0));

 private:
  static void InitCommandPool(const VkPhysicalDevice& physical_device, const VkDevice& device, const VkSurfaceKHR& surface,
                              const VkAllocationCallbacks* allocator);
  static void InitCommandBuffers(const VkDevice& device);
};
}  // namespace prt

#endif  // PRT_COMMAND_POOL_H
