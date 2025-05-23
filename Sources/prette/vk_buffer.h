#ifndef PRT_VK_BUFFER_H
#define PRT_VK_BUFFER_H

#include <array>
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/copy_to_buffer.h"
#include "prette/relaxed_atomic.h"
#include "prette/vk.h"

namespace prt {
class Driver;

namespace vk {
static constexpr const auto kDefaultTransferDestBufferMemoryFlags =
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
class BufferBuilder : public vk::HandleBuilderTemplate<VkBufferCreateInfo, Buffer> {
 private:
  VkMemoryPropertyFlags mem_flags_ = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

 public:
  BufferBuilder();
  ~BufferBuilder() override = default;

  auto WithSize(const VkDeviceSize rhs) -> BufferBuilder& {
    ASSERT(rhs > 0);
    info_ptr()->size = rhs;
    return *this;
  }

  auto WithSharingMode(const VkSharingMode rhs) -> BufferBuilder& {
    info_ptr()->sharingMode = rhs;
    return *this;
  }

  auto WithFlags(const VkBufferCreateFlags rhs) -> BufferBuilder& {
    info_ptr()->flags = rhs;
    return *this;
  }

  inline auto WithExclusiveSharingMode() -> BufferBuilder& {
    return WithSharingMode(VK_SHARING_MODE_EXCLUSIVE);
  }

  auto GetUsage() const -> VkBufferUsageFlags {
    return info().usage;
  }

  auto WithUsage(const VkBufferUsageFlags rhs) -> BufferBuilder& {
    info_ptr()->usage = rhs;
    return *this;
  }

  inline auto WithVertexBufferUsage() -> BufferBuilder& {
    return WithUsage(GetUsage() | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  }

  inline auto WithIndexBufferUsage() -> BufferBuilder& {
    return WithUsage(GetUsage() | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
  }

  inline auto WithUniformBufferUsage() -> BufferBuilder& {
    return WithUsage(GetUsage() | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
  }

  inline auto WithTransferDestUsage() -> BufferBuilder& {
    return WithUsage(GetUsage() | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
  }

  inline auto WithTransferSourceUsage() -> BufferBuilder& {
    return WithUsage(GetUsage() | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  }

  auto WithMemoryFlags(const VkMemoryPropertyFlags rhs) -> BufferBuilder& {
    mem_flags_ = rhs;
    return *this;
  }

  auto GetMemoryFlags() const -> VkMemoryPropertyFlags {
    return mem_flags_;
  }

  auto IsValid() const -> bool override;
  auto Build() -> Buffer* override;

  auto operator()() -> Buffer* {
    return Build();
  }

  operator Buffer*() {
    return Build();
  }

  inline auto BuildTransferDestBuffer(const VkDeviceSize size,
                                      const VkMemoryPropertyFlags mem_flags = kDefaultTransferDestBufferMemoryFlags)
      -> Buffer* {
    // clang-format off
        return WithSize(size)
          .WithTransferDestUsage()
          .WithMemoryFlags(mem_flags);
    // clang-format on
  }

  inline auto BuildTransferSourceBuffer(const VkDeviceSize size,
                                        const VkMemoryPropertyFlags mem_flags = kDefaultTransferDestBufferMemoryFlags)
      -> Buffer* {
    // clang-format off
    return WithSize(size)
      .WithTransferSourceUsage()
      .WithMemoryFlags(mem_flags);
    // clang-format on
  }

  auto BuildStagingBuffer(const uint8_t* data, const uint64_t num_bytes,
                          const VkMemoryPropertyFlags mem_flags = kDefaultTransferDestBufferMemoryFlags) -> Buffer*;
};

class Buffer {
  friend class BufferBuilder;

 public:
  static constexpr const VkMemoryPropertyFlags kDefaultBufferMemoryProperties =
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  static void AllocateImageMemory(Driver* driver, const VkImage& image, VkDeviceMemory& bufferMemory,
                                  VkMemoryPropertyFlags properties);
  static void CopyDataToImageWithStaging(const VkImage& image, const uint8_t* data, const uint64_t num_bytes,
                                         const std::vector<VkBufferImageCopy>& regions);

 private:
  static void AllocateMemory(Driver* driver, const VkDeviceSize alloc_size, const uint32_t memory_type,
                             VkDeviceMemory& memory);
  static void AllocateMemory(Driver* driver, const VkMemoryRequirements& mem_requirements, VkDeviceMemory& memory,
                             VkMemoryPropertyFlags properties);

  static inline void InitDescriptor(VkDescriptorBufferInfo& descriptor, const VkBuffer& buffer,
                                    const VkDeviceSize size = VK_WHOLE_SIZE, const VkDeviceSize offset = 0) {
    descriptor.buffer = buffer;
    descriptor.offset = offset;
    descriptor.range = size;
  }

 private:
  VkBuffer buffer_{};
  VkDeviceMemory memory_{};
  VkDeviceSize size_;
  VkDescriptorBufferInfo descriptor_{};

 protected:
  explicit Buffer(const VkDeviceSize size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties);
  explicit Buffer(const VkBufferCreateInfo& create_info, const VkMemoryPropertyFlags mem_flags);

 public:
  virtual ~Buffer();

  auto GetBuffer() const -> const VkBuffer& {
    return buffer_;
  }

  auto GetHandle() const -> const VkBuffer& {
    return buffer_;
  }

  auto GetMemory() const -> const VkDeviceMemory& {
    return memory_;
  }

  auto GetSize() const -> const VkDeviceSize& {
    return size_;
  }

  auto GetDescriptor() const -> const VkDescriptorBufferInfo& {
    return descriptor_;
  }

  auto IsInitialized() const -> bool {
    return buffer_ != VK_NULL_HANDLE;
  }

  void Destroy();

  operator VkBuffer() const {
    return buffer_;
  }

 public:
  static inline auto New(const VkDeviceSize size, const VkBufferUsageFlags usage,
                         const VkMemoryPropertyFlags properties = kDefaultBufferMemoryProperties) -> Buffer* {
    return new Buffer(size, usage, properties);
  }

  static inline auto NewUniformBuffer(const VkDeviceSize size,
                                      const VkMemoryPropertyFlags properties = kDefaultBufferMemoryProperties)
      -> Buffer* {
    return New(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, properties);
  }

  template <typename T>
  static inline auto NewUniformBuffer(const VkMemoryPropertyFlags properties = kDefaultBufferMemoryProperties)
      -> Buffer* {
    return NewUniformBuffer(sizeof(T), properties);
  }

  static inline auto NewTransferSource(const VkDeviceSize size,
                                       const VkMemoryPropertyFlags properties = kDefaultBufferMemoryProperties)
      -> Buffer* {
    return new Buffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, properties);
  }

  static inline auto NewStaging(const VkDeviceSize size,
                                const VkMemoryPropertyFlags properties = kDefaultBufferMemoryProperties) -> Buffer* {
    return New(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, properties);
  }

  static inline auto NewVertex(const VkDeviceSize size,
                               const VkMemoryPropertyFlags properties = kDefaultBufferMemoryProperties) -> Buffer* {
    return New(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, properties);
  }

  static inline auto NewIndex(const VkDeviceSize size,
                              const VkMemoryPropertyFlags properties = kDefaultBufferMemoryProperties) -> Buffer* {
    return New(size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, properties);
  }
};

static constexpr const auto kDefaultBufferSharingMode = VK_SHARING_MODE_EXCLUSIVE;

template <typename E, const VkDeviceSize MaxNumberOfElements, const VkBufferUsageFlags Usage,
          const VkSharingMode SharingMode = kDefaultBufferSharingMode>
class BufferTemplate : public Buffer {
  static constexpr const auto kElementSize = sizeof(E);
  static constexpr const auto kMaxNumberOfElements = MaxNumberOfElements;
  using ElementArray = std::array<E, MaxNumberOfElements>;

 public:
  static constexpr const auto kDefaultMemoryFlags = Buffer::kDefaultBufferMemoryProperties;
  static constexpr const auto kBufferUsage = Usage;
  static constexpr const auto kSharingMode = SharingMode;
  static constexpr const auto kTotalBufferSize = kElementSize * kMaxNumberOfElements;
  static constexpr const auto kHasTransferDstUsage =
      (Usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT) == VK_BUFFER_USAGE_TRANSFER_DST_BIT;

 private:
  ElementArray data_{};
  RelaxedAtomic<bool> dirty_ = true;

 protected:
  BufferTemplate(const VkBufferCreateInfo& create_info, const VkMemoryPropertyFlags mem_flags) :
    Buffer(create_info, mem_flags) {}
  BufferTemplate(const VkBufferUsageFlags usage, const VkMemoryPropertyFlags mem_flags = kDefaultMemoryFlags) :
    BufferTemplate(
        VkBufferCreateInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .size = kTotalBufferSize,
            .usage = kBufferUsage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        },
        mem_flags) {}
  BufferTemplate(const ElementArray& data, const VkMemoryPropertyFlags mem_flags = kDefaultMemoryFlags,
                 const bool staging = kHasTransferDstUsage) :
    BufferTemplate(
        VkBufferCreateInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .size = kTotalBufferSize,
            .usage = kBufferUsage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        },
        mem_flags) {
    SyncData(data);
  }

  void SetDirty(const bool rhs) {
    dirty_ = rhs;
  }

  inline void SyncFrom(const ElementArray& data, const bool staging = kHasTransferDstUsage) {
    if (staging) {
      vk::CopyBytesToBufferWithStaging copy((const uint8_t*)data.data(), data.size() * kElementSize);
      return copy(this);
    } else {
      vk::CopyBytesToBuffer copy((const uint8_t*)data.data(), data.size() * kElementSize);
      return copy(this);
    }
  }

  inline void SyncFromData(const bool staging = kHasTransferDstUsage) {
    return SyncFrom(data_, staging);
  }

 public:
  ~BufferTemplate() override = default;

  auto GetData() const -> const std::vector<E>& {
    return data_;
  }

  auto IsDirty() const -> bool {
    return (bool)dirty_;
  }

  auto GetSize() const -> uint64_t {
    return data_.size();
  }

  auto GetBufferSize() const -> VkDeviceSize {
    return GetSize() * sizeof(E);
  }

  auto GetCapacity() const -> VkDeviceSize {
    return data_.capacity();
  }

  auto GetBufferCapacity() const -> VkDeviceSize {
    return GetCapacity() * sizeof(E);
  }

  void SetData(const ElementArray& rhs) {
    data_ = rhs;
    dirty_ = true;
  }

  void SyncData(const ElementArray& rhs, const bool staging = kHasTransferDstUsage) {
    data_ = rhs;
    return SyncFromData(staging);
  }

  void SyncData(const E& rhs, const bool staging = kHasTransferDstUsage) {
    data_[0] = rhs;
    return SyncFromData(staging);
  }

  void Sync(const bool staging = kHasTransferDstUsage) {
    if (IsDirty())
      return SyncFromData(staging);
  }

  inline void MarkDirty() {
    return SetDirty(true);
  }

  inline void UnmarkDirty() {
    return SetDirty(false);
  }
};

template <typename T, const VkBufferUsageFlags BufferUsage, class B>
class BaseBufferBuilderTemplate : public HandleBuilderTemplate<VkBufferCreateInfo, T> {
  using Parent = HandleBuilderTemplate<VkBufferCreateInfo, T>;

 private:
  VkMemoryPropertyFlags mem_flags_ = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

  auto WithUsage(const VkBufferUsageFlags rhs) -> B& {
    Parent::info_ptr()->usage = rhs;
    return (B&)*this;
  }

 protected:
  explicit BaseBufferBuilderTemplate(const VkDeviceSize init_cap) :
    Parent() {
    Parent::info_ptr()->sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    Parent::info_ptr()->size = init_cap;
    Parent::info_ptr()->usage = BufferUsage;
  }

 public:
  auto WithFlags(const VkBufferCreateFlags rhs) -> B& {
    Parent::info_ptr()->flags = rhs;
    return *this;
  }

  auto WithSharingMode(const VkSharingMode rhs) -> B& {
    Parent::info_ptr()->sharingMode = rhs;
    return *this;
  }

  inline auto WithExclusiveSharingMode() -> B& {
    return WithSharingMode(VK_SHARING_MODE_EXCLUSIVE);
  }

  auto GetUsage() const -> VkBufferUsageFlags {
    return Parent::info().usage;
  }

  inline auto WithTransferDestUsage() -> B& {
    return WithUsage(GetUsage() | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
  }

  inline auto WithTransferSourceUsage() -> B& {
    return WithUsage(GetUsage() | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  }

  auto WithMemoryFlags(const VkMemoryPropertyFlags rhs) -> B& {
    Parent::info_ptr()->flags = rhs;
    return *this;
  }

  auto GetMemoryFlags() const -> VkMemoryPropertyFlags {
    return mem_flags_;
  }
};
}  // namespace vk
}  // namespace prt

#endif  // PRT_VK_BUFFER_H
