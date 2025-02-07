#include <vulkan/vulkan_core.h>

#include "prette/command_pool.h"
#include "prette/common.h"
#include "prette/gfx.h"
#ifdef PRT_VK

#include "prette/command_pool.h"
#include "prette/runtime.h"

namespace prt::vk {
void Buffer::AllocateMemory(const VkDevice& device, const VkDeviceSize alloc_size, const uint32_t memory_type,
                            VkDeviceMemory& memory, const VkAllocationCallbacks* allocator) {
  VkMemoryAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = alloc_size;
  alloc_info.memoryTypeIndex = memory_type;
  CHECK_VK(FATAL, vkAllocateMemory(device, &alloc_info, nullptr, &memory), "failed to allocate vk memory");
  ASSERT(memory != VK_NULL_HANDLE);
}

Buffer::Buffer(const VkDeviceSize size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties) :
  size_(size) {
  VkBufferCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  create_info.size = size;
  create_info.usage = usage;
  create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  CHECK_VK(FATAL, vkCreateBuffer(Runtime::GetVkLogicalDevice(), &create_info, nullptr, &buffer_), "failed to create vk buffer");

  const auto& device = Runtime::GetVkLogicalDevice();
  VkMemoryRequirements mem_requirements{};
  vkGetBufferMemoryRequirements(device, buffer_, &mem_requirements);
  const auto memory_type = FindMemoryType(Runtime::GetVkPhysicalDevice(), mem_requirements.memoryTypeBits, properties);
  AllocateMemory(device, size, memory_type, memory_, nullptr);
  CHECK_VK(FATAL, vkBindBufferMemory(device, buffer_, memory_, 0), "failed to bind vk buffer memory");
  InitDescriptor(descriptor_, buffer_);
}

Buffer::~Buffer() {
  Destroy();
}

class StagingBufferScope {
 private:
  Buffer* buffer_;
  MappedBufferScope mapped_;

 public:
  StagingBufferScope(const uint64_t num_bytes) :
    buffer_(Buffer::NewTransferSource(num_bytes)),
    mapped_(buffer_) {
    ASSERT(buffer_);
  }
  ~StagingBufferScope() {
    mapped_.~MappedBufferScope();
    delete buffer_;
  }

  auto GetBuffer() const -> Buffer* {
    return buffer_;
  }

  inline auto HasBuffer() const -> bool {
    return GetBuffer() != nullptr;
  }

  inline auto IsMapped() const -> bool {
    return mapped_;
  }

  void CopyFrom(const void* src, const uint64_t num_bytes) {
    ASSERT(IsMapped());
    ASSERT(num_bytes >= 0);  // TODO: check upper bounds
    return mapped_.CopyFrom(src, num_bytes);
  }

  operator bool() const {
    return HasBuffer() && IsMapped();
  }
};

void Buffer::CopyFromBytes(const void* data, const uint64_t num_bytes, const bool staging) {
  ASSERT(data);
  ASSERT(num_bytes >= 1);
  ASSERT(memory_ != VK_NULL_HANDLE);
  if (staging) {
    StagingBufferScope staging(num_bytes);
    ASSERT(staging);
    staging.CopyFrom(data, num_bytes);
    CopyFromBuffer(staging.GetBuffer(), num_bytes);
  } else {
    MappedBufferScope mapped(this);
    ASSERT(mapped);
    mapped.CopyFrom(data, num_bytes);
  }
}

class SingleUseCommandBuffer {
 private:
  VkCommandBuffer buffer_{};

  static inline void InitCommandBuffer(const VkDevice& device, const VkCommandPool& command_pool, VkCommandBuffer& buffer) {
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = command_pool;
    alloc_info.commandBufferCount = 1;
    CHECK_VK(FATAL, vkAllocateCommandBuffers(device, &alloc_info, &buffer), "failed to allocate single use vk command buffer");
  }

  static inline void StartCommandBuffer(const VkCommandBuffer& buffer) {
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    CHECK_VK(FATAL, vkBeginCommandBuffer(buffer, &begin_info), "failed to begin vk command buffer");
  }

  static inline void SubmitCommandBuffer(const VkQueue& queue, const VkCommandBuffer& buffer) {
    CHECK_VK(FATAL, vkEndCommandBuffer(buffer), "failed to end vk command buffer");

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &buffer;
    vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);
  }

  static inline void DestroyCommandBuffer(const VkDevice& device, const VkCommandPool& command_pool,
                                          const VkCommandBuffer& buffer) {
    vkFreeCommandBuffers(device, command_pool, 1, &buffer);
  }

 public:
  SingleUseCommandBuffer() {
    InitCommandBuffer(Runtime::GetVkLogicalDevice(), CommandPool::GetCommandPool(), buffer_);
    StartCommandBuffer(buffer_);
  }
  ~SingleUseCommandBuffer() {
    SubmitCommandBuffer(Runtime::GetVkGraphicsQueue(), buffer_);
    DestroyCommandBuffer(Runtime::GetVkLogicalDevice(), CommandPool::GetCommandPool(), buffer_);
  }

  operator VkCommandBuffer() const {
    return buffer_;
  }
};

void Buffer::CopyFromBuffer(const VkBuffer& src, const VkDeviceSize num_bytes) {
  SingleUseCommandBuffer buffer;
  VkBufferCopy copy{};
  copy.size = num_bytes;
  vkCmdCopyBuffer(buffer, src, buffer_, 1, &copy);
}

void Buffer::Destroy() {
  const auto& device = Runtime::GetVkLogicalDevice();
  if (buffer_)
    vkDestroyBuffer(device, buffer_, nullptr);
  if (memory_)
    vkFreeMemory(device, memory_, nullptr);
}

void Buffer::Flush(const VkDeviceSize size, const VkDeviceSize offset) {
  VkMappedMemoryRange range{};
  range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  range.memory = memory_;
  range.offset = offset;
  range.size = size;
  CHECK_VK(FATAL, vkFlushMappedMemoryRanges(Runtime::GetVkLogicalDevice(), 1, &range), "failed to flush vk buffer");
}

MappedBufferScope::MappedBufferScope(const Buffer* buffer, const VkDeviceSize size, const VkDeviceSize offset,
                                     const VkMemoryMapFlags flags) :
  buffer_(buffer) {
  ASSERT(buffer_);
  CHECK_VK(FATAL, vkMapMemory(Runtime::GetVkLogicalDevice(), GetBuffer()->GetMemory(), offset, size, flags, &mapped_memory_),
           "failed to map vk buffer memory");
}

MappedBufferScope::~MappedBufferScope() {
  if (!IsMapped())
    return;
  ASSERT(GetBuffer()->GetMemory() != VK_NULL_HANDLE);
  vkUnmapMemory(Runtime::GetVkLogicalDevice(), GetBuffer()->GetMemory());
  mapped_memory_ = nullptr;
}

void MappedBufferScope::CopyFrom(const void* src, const VkDeviceSize num_bytes) {
  memcpy(mapped_memory_, src, num_bytes == VK_WHOLE_SIZE ? GetBuffer()->GetSize() : num_bytes);
}
}  // namespace prt::vk

#endif  // PRT_VK