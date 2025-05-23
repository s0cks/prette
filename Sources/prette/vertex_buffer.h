#ifndef PRT_VERTEX_BUFFER_H
#define PRT_VERTEX_BUFFER_H

#include <array>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/platform.h"
#include "prette/vertex.h"
#include "prette/vk.h"
#include "prette/vk_buffer.h"

namespace prt::vk {
template <VertexType V, const uint64_t MaxNumberOfVertices>
class VertexBufferBuilder;

template <VertexType V, const uint64_t MaxNumberOfVertices,
          const VkBufferUsageFlags Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          const VkSharingMode SharingMode = kDefaultBufferSharingMode>
class VertexBuffer : public BufferTemplate<V, MaxNumberOfVertices, Usage, SharingMode> {
  friend class VertexBufferBuilder<V, MaxNumberOfVertices>;
  using Parent = BufferTemplate<V, MaxNumberOfVertices, Usage, SharingMode>;

 public:
  using VertexArray = std::array<V, MaxNumberOfVertices>;
  static constexpr const auto kVertexSize = sizeof(V);

 private:
  VertexBuffer(const VkBufferCreateInfo& create_info, const VkMemoryPropertyFlags mem_flags) :
    Parent(create_info, mem_flags) {}

 public:
  explicit VertexBuffer(const VkMemoryPropertyFlags mem_flags = Parent::kDefaultMemoryFlags) :
    Parent(mem_flags) {}
  explicit VertexBuffer(const VertexArray& data, const VkMemoryPropertyFlags mem_flags = Parent::kDefaultMemoryFlags,
                        const bool staging = Parent::kHasTransferDstUsage) :
    Parent(data, mem_flags, staging) {}
  ~VertexBuffer() override = default;
};

// template <VertexType V, const uint64_t MaxNumberOfVertices>
// class VertexBufferBuilder :
//   public BaseBufferBuilderTemplate<VertexBuffer<V, MaxNumberOfVertices>, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
//                                    VertexBufferBuilder<V, MaxNumberOfVertices>> {
//   using Parent = BaseBufferBuilderTemplate<VertexBuffer<V, MaxNumberOfVertices>, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
//                                            VertexBufferBuilder<V, MaxNumberOfVertices>>;
//   using Builder = VertexBufferBuilder<V, MaxNumberOfVertices>;
//   static constexpr const auto kVertexSize = sizeof(V);
//   static constexpr const auto kTotalBufferSize = kVertexSize * MaxNumberOfVertices;

//  protected:
//   VertexBufferBuilder() :
//     Parent(kTotalBufferSize) {}

//   auto IsValid() const -> bool override {
//     NOT_IMPLEMENTED(ERROR);
//     return true;
//   }

//   auto Build() -> VertexBuffer<V, MaxNumberOfVertices>* override {
//     ASSERT(Parent::IsValid());
//     return new VertexBuffer<V, MaxNumberOfVertices>(Parent::info(), Parent::GetMemoryFlags());
//   }
// };
}  // namespace prt::vk

#endif  // PRT_VERTEX_BUFFER_H
