#ifndef PRT_VK_BUFFER_H
#define PRT_VK_BUFFER_H

#include <vulkan/vulkan_core.h>

#include "prette/alloc_mem_req.h"
#include "prette/assertions.h"
#include "prette/vk.h"

namespace prt {
class Driver;

namespace vk {
static constexpr const auto kDefaultBufferSharingMode = VK_SHARING_MODE_EXCLUSIVE;
static constexpr const auto kDefaultBufferMemoryProperties =
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

class Buffer {
  friend class BaseBufferBuilder;

 private:
  VkBuffer buffer_{};
  VkDeviceMemory memory_{};
  VkDeviceSize size_;
  VkDescriptorBufferInfo descriptor_{};

  inline void SetMemory(const VkDeviceMemory& rhs) {
    ASSERT(rhs != VK_NULL_HANDLE);
    memory_ = rhs;
  }

  void InitMemory(AllocMemoryRequest& alloc_mem);
  void BindMemory();

  static inline void InitDescriptor(VkDescriptorBufferInfo& descriptor, const VkBuffer& buffer,
                                    const VkDeviceSize size = VK_WHOLE_SIZE, const VkDeviceSize offset = 0) {
    descriptor.buffer = buffer;
    descriptor.offset = offset;
    descriptor.range = size;
  }

 protected:
  Buffer(const VkBufferCreateInfo& create_info, const VkDeviceMemory memory);
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
};

class BaseBufferBuilder {
 protected:
  VkBufferCreateInfo info_{};

  BaseBufferBuilder() {
    info_ptr()->sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info_ptr()->sharingMode = kDefaultBufferSharingMode;
    info_ptr()->flags = kDefaultBufferMemoryProperties;
    info_ptr()->pNext = nullptr;
  }

  inline auto info_ptr() -> VkBufferCreateInfo* {
    return &info_;
  }

  inline void SetUsage(const VkBufferUsageFlags rhs) {
    info_ptr()->usage = rhs;
  }

 public:
  virtual ~BaseBufferBuilder();

  inline auto info() const -> const VkBufferCreateInfo& {
    return info_;
  }

  auto GetUsage() const -> VkBufferUsageFlags {
    return info().usage;
  }

  auto GetSharingMode() const -> VkSharingMode {
    return info().sharingMode;
  }

  auto GetBufferSize() const -> VkDeviceSize {
    return info().size;
  }

  virtual auto IsValid() const -> bool;
  virtual auto Build(const VkMemoryPropertyFlags mem_flags = kDefaultBufferMemoryProperties) -> Buffer*;
};

class BufferBuilder : public BaseBufferBuilder {
 public:
  auto WithSharingMode(const VkSharingMode rhs) -> BufferBuilder& {
    info_ptr()->sharingMode = rhs;
    return *this;
  }

  inline auto WithTransferSourceUsage() -> BufferBuilder& {
    SetUsage(GetUsage() | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    return *this;
  }

  inline auto WithTransferDestUsage() -> BufferBuilder& {
    SetUsage(GetUsage() | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    return *this;
  }

  inline auto WithExclusiveSharingMode() -> BufferBuilder& {
    return WithSharingMode(VK_SHARING_MODE_EXCLUSIVE);
  }
};

template <class E, const VkBufferUsageFlags Usage, class B>
class BaseBufferBuilderTemplate : public BaseBufferBuilder {
 protected:
  explicit BaseBufferBuilderTemplate(const VkDeviceSize init_length = 0) :
    BaseBufferBuilder() {
    SetUsage(Usage);
    SetLength(init_length);
  }

  inline void SetBufferSize(const VkDeviceSize rhs) {
    info_ptr()->size = rhs;
  }

  inline void SetLength(const VkDeviceSize rhs) {
    return SetBufferSize(sizeof(E) * rhs);
  }

 public:
  ~BaseBufferBuilderTemplate() override = default;

  auto WithSharingMode(const VkSharingMode rhs) -> B& {
    info_ptr()->sharingMode = rhs;
    return (B&)(*this);
  }

  inline auto WithTransferSourceUsage() -> B& {
    SetUsage(GetUsage() | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    return (B&)(*this);
  }

  inline auto WithTransferDestUsage() -> B& {
    SetUsage(GetUsage() | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    return (B&)(*this);
  }

  inline auto WithExclusiveSharingMode() -> B& {
    return (B&)WithSharingMode(VK_SHARING_MODE_EXCLUSIVE);
  }

  auto WithLength(const VkDeviceSize rhs) -> B& {
    SetLength(rhs);
    return (B&)(*this);
  }
};
}  // namespace vk
}  // namespace prt

#endif  // PRT_VK_BUFFER_H
