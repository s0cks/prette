#ifndef PRT_UNIFORM_BUFFER_H
#define PRT_UNIFORM_BUFFER_H

#include <array>
#include <type_traits>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/vk_buffer.h"

namespace prt::vk {
template <typename T>
struct is_uniform_t : std::false_type {};

template <typename T>
concept UniformType = is_uniform_t<T>::value;

template <UniformType U, const uint64_t MaxNumberOfUniforms,
          const VkBufferUsageFlags Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          const VkSharingMode SharingMode = kDefaultBufferSharingMode>
class UniformBuffer : public BufferTemplate<U, MaxNumberOfUniforms, Usage, SharingMode> {
  using Parent = BufferTemplate<U, MaxNumberOfUniforms, Usage, SharingMode>;

 public:
  using UniformArray = std::array<U, MaxNumberOfUniforms>;
  static constexpr const auto kUniformSize = sizeof(U);

 private:
  UniformBuffer(const VkBufferCreateInfo& create_info, const VkMemoryPropertyFlags mem_flags) :
    Parent(create_info, mem_flags) {}

 public:
  explicit UniformBuffer(const VkMemoryPropertyFlags mem_flags = Parent::kDefaultMemoryFlags) :
    Parent(mem_flags) {}
  explicit UniformBuffer(const UniformArray& data, const VkMemoryPropertyFlags mem_flags = Parent::kDefaultMemoryFlags,
                         const bool staging = Parent::kHasTransferDstUsage) :
    Parent(data, mem_flags) {}
  ~UniformBuffer() override = default;
};

// template <UniformType U, const uint64_t MaxNumberOfUniforms>
// class UniformBufferBuilder :
//   public BaseBufferBuilderTemplate<UniformBuffer<U, MaxNumberOfUniforms>, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//                                    UniformBufferBuilder<U, MaxNumberOfUniforms>> {
//  public:
//   using BufferType = UniformBuffer<U, MaxNumberOfUniforms>;
//   using BuilderType = UniformBufferBuilder<U, MaxNumberOfUniforms>;
//   static constexpr const auto kElementSize = sizeof(U);
//   static constexpr const auto kTotalBufferSize = kElementSize * MaxNumberOfUniforms;

//  private:
//   using Parent = BaseBufferBuilderTemplate<BufferType, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, BuilderType>;

//  public:
//   UniformBufferBuilder() :
//     Parent(kTotalBufferSize) {}
//   ~UniformBufferBuilder() override = default;

//   auto IsValid() const -> bool override {
//     NOT_IMPLEMENTED(ERROR);
//     return true;
//   }

//   auto Build() -> BufferType* override {
//     ASSERT(IsValid());
//     return new UniformBuffer<U, MaxNumberOfUniforms>(Parent::info(), Parent::GetMemoryFlags());
//   }
// };
}  // namespace prt::vk

#endif  // PRT_UNIFORM_BUFFER_H
