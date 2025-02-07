#ifndef PRT_PIPELINE_H
#define PRT_PIPELINE_H

#include <vulkan/vulkan_core.h>

#include "prette/gfx.h"

namespace prt {
class Pipeline {
 private:
  static void DestroyPipeline(const VkDevice& device, const VkAllocationCallbacks* allocator = nullptr);
  static void DestroyPipelineLayout(const VkDevice& device, const VkAllocationCallbacks* allocator = nullptr);

 public:
  static void Init(const VkDevice& device);
  static auto GetPipeline() -> const VkPipeline&;
  static auto GetPipelineLayout() -> const VkPipelineLayout&;
  static void Shutdown(const VkDevice& device, const VkAllocationCallbacks* allocator = nullptr);
};
}  // namespace prt

#endif  // PRT_PIPELINE_H
