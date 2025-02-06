#ifndef PRT_RENDERER_H
#define PRT_RENDERER_H

#include <vector>

#include "prette/gfx.h"

namespace prt {
class Renderer {
 private:
  VkInstance instance_{};
  VkDevice device_{};
  VkPhysicalDevice physical_device_{};
  VkQueue graphics_queue_{};
  VkQueue present_queue_{};
  VkExtent2D swap_extent_{};
  VkExtent2D deferred_extent_{};
  VkCommandPool command_pool_{};
  VkSurfaceKHR surface_{};

  std::vector<VkDrawIndexedIndirectCommand> render_commands_{};
  MemoryBuffer indirect_{};
  uint32_t current_vtx_{};
  uint32_t current_idx_{};
  uint32_t num_meshes_{};

  VkPipelineCache pipeline_cache_{};
  VkPipeline pipeline_{};
  VkRenderPass render_pass_{};
  VkRenderPass deferred_pass_{};

  MemoryBuffer vertex_buffer_{};
  MemoryBuffer index_buffer_{};
};
}  // namespace prt

#endif  // PRT_RENDERER_H
