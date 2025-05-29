#ifndef PRT_UNIFORM_BUFFER_H
#define PRT_UNIFORM_BUFFER_H

#include <vulkan/vulkan_core.h>

#include "prette/std140.h"
#include "prette/vk_buffer.h"

namespace prt::vk {
static constexpr const auto kDefaultUniformBufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
template <std140::IsAligned U>
class UniformBufferBuilder : public BaseBufferBuilderTemplate<U, kDefaultUniformBufferUsage, UniformBufferBuilder<U>> {
 public:
  explicit UniformBufferBuilder(const VkDeviceSize init_length = 0) :
    BaseBufferBuilderTemplate<U, kDefaultUniformBufferUsage, UniformBufferBuilder<U>>(init_length) {}
  ~UniformBufferBuilder() override = default;

  operator vk::Buffer*() {
    return BaseBufferBuilderTemplate<U, kDefaultUniformBufferUsage, UniformBufferBuilder<U>>::Build();
  }
};
}  // namespace prt::vk

#endif  // PRT_UNIFORM_BUFFER_H
