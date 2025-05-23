#ifndef PRT_INDEX_BUFFER_H
#define PRT_INDEX_BUFFER_H

#include <array>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/index_class.h"
#include "prette/platform.h"
#include "prette/vk_buffer.h"

namespace prt::vk {
template <IndexType I, const uint64_t MaxNumberOfIndices,
          const VkBufferUsageFlags Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          const VkSharingMode SharingMode = kDefaultBufferSharingMode>
class IndexBuffer : public BufferTemplate<I, MaxNumberOfIndices, Usage, SharingMode> {
  using Parent = BufferTemplate<I, MaxNumberOfIndices, Usage, SharingMode>;

 public:
  using IndexClass = IndexClass<I>;
  using IndexArray = std::array<I, MaxNumberOfIndices>;

 private:
  IndexBuffer(const VkBufferCreateInfo& create_info, const VkMemoryPropertyFlags mem_flags) :
    Parent(create_info, mem_flags) {}

 public:
  explicit IndexBuffer(const VkMemoryPropertyFlags mem_flags = Parent::kDefaultMemoryFlags) :
    Parent(mem_flags) {}
  explicit IndexBuffer(const IndexArray& data, const VkMemoryPropertyFlags mem_flags = Parent::kDefaultMemoryFlags,
                       const bool staging = Parent::kHasTransferDstUsage) :
    Parent(data, mem_flags) {}
  ~IndexBuffer() override = default;
};

// template <IndexType I, const uint64_t MaxNumberOfIndices>
// class IndexBufferBuilder :
//   public BaseBufferBuilderTemplate<IndexBuffer<I, MaxNumberOfIndices>,
//                                    IndexBuffer<I, MaxNumberOfIndices>::kDefaultUsage,
//                                    IndexBufferBuilder<I, MaxNumberOfIndices>> {
//  public:
//   using BufferType = IndexBuffer<I, MaxNumberOfIndices>;
//   using BuilderType = IndexBufferBuilder<I, MaxNumberOfIndices>;
//   using IndexClass = IndexClassType<I>;
//   static constexpr const auto kIndexSize = sizeof(I);
//   static constexpr const auto kTotalBufferSize = kIndexSize * MaxNumberOfIndices;

//  private:
//   using Parent = BaseBufferBuilderTemplate<BufferType, BufferType::kDefaultUsage, BuilderType>;

//  public:
//   IndexBufferBuilder() :
//     Parent(kTotalBufferSize) {}

//   auto IsValid() const -> bool override {
//     NOT_IMPLEMENTED(ERROR);
//     return true;
//   }

//   auto Build() -> BufferType* override {
//     ASSERT(Parent::IsValid());
//     return new IndexBuffer<I, MaxNumberOfIndices>(Parent::info(), Parent::GetMemoryFlags());
//   }
// };
}  // namespace prt::vk

#endif  // PRT_INDEX_BUFFER_H
