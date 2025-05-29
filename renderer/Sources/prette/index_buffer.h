#ifndef PRT_INDEX_BUFFER_H
#define PRT_INDEX_BUFFER_H

#include <cstdint>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/index_class.h"
#include "prette/vk_buffer.h"

namespace prt::vk {
static constexpr const auto kDefaultIndexBufferUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
template <IndexType I>
class IndexBufferBuilder : public BaseBufferBuilderTemplate<I, kDefaultIndexBufferUsage, IndexBufferBuilder<I>> {
 public:
  explicit IndexBufferBuilder(const VkDeviceSize init_length = 0) :
    BaseBufferBuilderTemplate<I, kDefaultIndexBufferUsage, IndexBufferBuilder<I>>(init_length) {}
  ~IndexBufferBuilder() override = default;

  auto operator()() -> Buffer* {
    return BaseBufferBuilderTemplate<I, kDefaultIndexBufferUsage, IndexBufferBuilder<I>>::Build();
  }
};

template <IndexType I>
static inline void BindIndexBuffer(VkCommandBuffer& cmd, vk::Buffer* buffer, const uint64_t offset = 0) {
  ASSERT(buffer);
  vkCmdBindIndexBuffer(cmd, buffer->GetBuffer(), offset, IndexClass<I>::kFormat);
}
}  // namespace prt::vk

#endif  // PRT_INDEX_BUFFER_H
