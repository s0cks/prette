#ifndef PRT_VERTEX_BUFFER_H
#define PRT_VERTEX_BUFFER_H

#include <vulkan/vulkan_core.h>

#include "prette/vertex/vertex.h"
#include "prette/vk_buffer.h"

namespace prt::vk {
template <VertexType V>
class VertexBufferBuilder :
  public BaseBufferBuilderTemplate<V, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VertexBufferBuilder<V>> {
 public:
  explicit VertexBufferBuilder(const VkDeviceSize init_length = 0) :
    BaseBufferBuilderTemplate<V, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VertexBufferBuilder<V>>(init_length) {}
  ~VertexBufferBuilder() override = default;

  auto operator()() -> Buffer* {
    return BaseBufferBuilderTemplate<V, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VertexBufferBuilder<V>>::Build();
  }
};
}  // namespace prt::vk

#endif  // PRT_VERTEX_BUFFER_H
