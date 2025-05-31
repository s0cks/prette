#ifndef PRT_VERTEX_BUFFER_H
#define PRT_VERTEX_BUFFER_H

#include <array>
#include <cstdint>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/vertex/vertex.h"
#include "prette/vk_buffer.h"

namespace prt::vk {
static constexpr const auto kDefaultVertexBufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
template <VertexType V>
class VertexBufferBuilder : public BaseBufferBuilderTemplate<V, kDefaultVertexBufferUsage, VertexBufferBuilder<V>> {
 public:
  explicit VertexBufferBuilder(const VkDeviceSize init_length = 0) :
    BaseBufferBuilderTemplate<V, kDefaultVertexBufferUsage, VertexBufferBuilder<V>>(init_length) {}
  ~VertexBufferBuilder() override = default;

  auto operator()() -> Buffer* {
    return BaseBufferBuilderTemplate<V, kDefaultVertexBufferUsage, VertexBufferBuilder<V>>::Build();
  }

  operator Buffer*() {
    return BaseBufferBuilderTemplate<V, kDefaultVertexBufferUsage, VertexBufferBuilder<V>>::Build();
  }
};

static inline void BindVertexBuffer(VkCommandBuffer& cmd, vk::Buffer* buffer, const uint64_t first_binding = 0,
                                    const uint64_t num_bindings = 1, const uint64_t offset = 0) {
  ASSERT(buffer);
  std::array<VkDeviceSize, 1> offsets = {offset};
  std::array<VkBuffer, 1> buffers = {buffer->GetBuffer()};
  vkCmdBindVertexBuffers(cmd, first_binding, num_bindings, buffers.data(), offsets.data());
}
}  // namespace prt::vk

#endif  // PRT_VERTEX_BUFFER_H
