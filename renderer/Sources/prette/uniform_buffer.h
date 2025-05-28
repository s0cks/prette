#ifndef PRT_UNIFORM_BUFFER_H
#define PRT_UNIFORM_BUFFER_H

#include <vulkan/vulkan_core.h>

#include "prette/std140.h"
#include "prette/vk_buffer.h"

namespace prt::vk {
template <std140::IsAligned U>
class UniformBufferBuilder :
  public BaseBufferBuilderTemplate<U, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, UniformBufferBuilder<U>> {
 public:
  explicit UniformBufferBuilder(const VkDeviceSize init_length = 0) :
    BaseBufferBuilderTemplate<U, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, UniformBufferBuilder<U>>(init_length) {}
  ~UniformBufferBuilder() override = default;
};
}  // namespace prt::vk

#endif  // PRT_UNIFORM_BUFFER_H
