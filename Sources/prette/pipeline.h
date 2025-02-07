#ifndef PRT_PIPELINE_H
#define PRT_PIPELINE_H

#include <vulkan/vulkan_core.h>

#include <array>

#include "prette/gfx.h"

namespace prt {
struct Vertex {
  glm::vec2 pos;
  glm::vec3 color;

  static auto GetBindingDescription() -> VkVertexInputBindingDescription {
    VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(Vertex);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return binding;
  }

  static auto GetAttributeDescriptions() -> const std::array<VkVertexInputAttributeDescription, 2>& {
    static std::array<VkVertexInputAttributeDescription, 2> attributes{};
    if (attributes.at(0).format != VK_FORMAT_R32G32_SFLOAT) {
      attributes.at(0).binding = 0;
      attributes.at(0).location = 0;
      attributes.at(0).format = VK_FORMAT_R32G32_SFLOAT;
      attributes.at(0).offset = offsetof(Vertex, pos);

      attributes.at(1).binding = 0;
      attributes.at(1).location = 1;
      attributes.at(1).format = VK_FORMAT_R32G32B32_SFLOAT;
      attributes.at(1).offset = offsetof(Vertex, color);
    }
    return attributes;
  }
};

class Pipeline {
  friend class Runtime;

 private:
  static void InitVertexBuffer(const VkPhysicalDevice& physical_device, const VkDevice& device,
                               const VkAllocationCallbacks* allocator = nullptr);
  static void InitIndexBuffer(const VkPhysicalDevice& physical_device, const VkDevice& device,
                              const VkAllocationCallbacks* allocator = nullptr);
  static void DestroyVertexBuffer(const VkDevice& device, const VkAllocationCallbacks* allocator = nullptr);
  static void DestroyIndexBuffer(const VkDevice& device, const VkAllocationCallbacks* allocator = nullptr);
  static void DestroyPipeline(const VkDevice& device, const VkAllocationCallbacks* allocator = nullptr);
  static void DestroyPipelineLayout(const VkDevice& device, const VkAllocationCallbacks* allocator = nullptr);

 public:
  static void Init(const VkDevice& device);
  static auto GetPipeline() -> const VkPipeline&;
  static auto GetPipelineLayout() -> const VkPipelineLayout&;
  static auto GetVertexBuffer() -> const VkBuffer&;
  static auto GetIndexBuffer() -> const VkBuffer&;
  static auto GetNumberOfIndices() -> uint32_t;
  static void Shutdown(const VkDevice& device, const VkAllocationCallbacks* allocator = nullptr);
};
}  // namespace prt

#endif  // PRT_PIPELINE_H
