#ifndef PRT_VK_CMD_BUFFERS_H
#define PRT_VK_CMD_BUFFERS_H

#include <algorithm>
#include <array>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/vk.h"

namespace prt::vk {
class AllocateCommandBuffers {
 public:
  static constexpr const auto kDefaultLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

 private:
  VkCommandBufferAllocateInfo info_{};

  inline auto info_ptr() -> VkCommandBufferAllocateInfo* {
    return &info_;
  }

 public:
  AllocateCommandBuffers(const uint32_t num_buffers = 0);
  ~AllocateCommandBuffers() = default;

  auto GetNumberOfBuffers() const -> uint32_t {
    return info_.commandBufferCount;
  }

  auto WithLevel(const VkCommandBufferLevel rhs) -> AllocateCommandBuffers& {
    info_ptr()->level = rhs;
    return *this;
  }

  auto WithNumberOfBuffers(const uint32_t rhs) -> AllocateCommandBuffers& {
    info_ptr()->commandBufferCount = rhs;
    return *this;
  }

  auto WithPool(VkCommandPool rhs) -> AllocateCommandBuffers& {
    info_ptr()->commandPool = rhs;
    return *this;
  }

  auto WithPool(CommandPool* rhs) -> AllocateCommandBuffers&;
  auto IsValid() const -> bool;
  auto Allocate(std::vector<VkCommandBuffer>& results) -> bool;
  auto Allocate(VkCommandBuffer* results) -> bool;
};

void FreeCommandBuffers(VkCommandBuffer* buffers, const uint64_t num_buffers);

template <typename Container>
static inline void FreeCommandBuffers(Container& buffers) {
  return FreeCommandBuffers(buffers.data(), buffers.size());
}

void Submit(const VkQueue& queue, const VkCommandBuffer* buffers, const uint64_t num_buffers, const VkFence& fence);

class SingleUseCommandBuffer {
  static void Start(const VkCommandBuffer& buffer);
  static void Finish(const VkCommandBuffer& buffer);
  static void Submit(const VkDevice& device, const VkQueue& queue, const VkCommandBuffer& buffer,
                     const VkAllocationCallbacks* allocator);

 private:
  VkCommandBuffer buffer_{};

 public:
  SingleUseCommandBuffer();
  ~SingleUseCommandBuffer();

  void Finish();

  operator VkCommandBuffer&() {
    return buffer_;
  }
};

static constexpr const auto kMaxCommandBufferPoolSize = MAX_NUMBER_OF_FRAMES_IN_FLIGHT + 1;
static constexpr const auto kDefaultCommandBufferPoolSize = kMaxCommandBufferPoolSize;
template <const int NumberOfBuffers = kDefaultCommandBufferPoolSize>
class CommandBufferPool {
  using Pool = std::array<VkCommandBuffer, NumberOfBuffers>;

  static_assert(NumberOfBuffers >= 1 && NumberOfBuffers <= kDefaultCommandBufferPoolSize,
                "expected NumberOfBuffers to be in the range of [0-kMaxCommandBufferPoolSize]");

 private:
  Pool buffers_{};

 public:
  explicit CommandBufferPool() :
    buffers_() {
    std::ranges::fill(buffers_, VK_NULL_HANDLE);
    AllocateCommandBuffers alloc(NumberOfBuffers);
    alloc.Allocate(&buffers_[0]);
  }
  ~CommandBufferPool() {
    FreeCommandBuffers(buffers_);
  }

  auto IsEmpty() const -> bool {
    return buffers_.empty();
  }

  auto GetSize() const -> uint64_t {
    return buffers_.size();
  }

  auto At(const uint64_t idx) -> VkCommandBuffer& {
    ASSERT(idx >= 0 && idx <= GetSize());
    return buffers_.at(idx);
  }

 public:
  static inline auto New() -> CommandBufferPool<NumberOfBuffers>* {
    return new CommandBufferPool<NumberOfBuffers>();
  }
};

class CommandBufferScope {
 private:
  VkCommandBufferBeginInfo begin_info_{};
  VkCommandBuffer* buffer_;

 public:
  explicit CommandBufferScope(VkCommandBuffer* buffer, const bool reset = false);
  ~CommandBufferScope();

  auto GetBeginInfo() const -> VkCommandBufferBeginInfo const& {
    return begin_info_;
  }

  operator VkCommandBuffer() const {
    return (*buffer_);
  }

  operator VkCommandBuffer*() const {
    return buffer_;
  }
};
}  // namespace prt::vk

#endif  // PRT_VK_CMD_BUFFERS_H
