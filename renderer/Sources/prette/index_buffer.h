#ifndef PRT_INDEX_BUFFER_H
#define PRT_INDEX_BUFFER_H

#include <vulkan/vulkan_core.h>

#include "prette/index_class.h"
#include "prette/vk_buffer.h"

namespace prt::vk {
template <IndexType I>
class IndexBufferBuilder :
  public BaseBufferBuilderTemplate<I, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, IndexBufferBuilder<I>> {
 public:
  explicit IndexBufferBuilder(const VkDeviceSize init_length = 0) :
    BaseBufferBuilderTemplate<I, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, IndexBufferBuilder<I>>(init_length) {}
  ~IndexBufferBuilder() override = default;

  auto operator()() -> Buffer* {
    return BaseBufferBuilderTemplate<I, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, IndexBufferBuilder<I>>::Build();
  }
};
}  // namespace prt::vk

#endif  // PRT_INDEX_BUFFER_H
