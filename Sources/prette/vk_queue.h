#ifndef PRT_VK_QUEUE_H
#define PRT_VK_QUEUE_H

#include <string>
#include <vector>

#include "prette/common.h"
#include "prette/vk.h"

namespace prt {
class Swapchain;
namespace vk {
class QueueSubmit {
 private:
  VkQueue queue_;
  VkSubmitInfo info_{};
  std::vector<VkSemaphore> wait_semaphores_{};
  std::vector<VkPipelineStageFlags> wait_stages_{};
  std::vector<VkSemaphore> signal_semaphores_{};
  std::vector<VkCommandBuffer> command_buffers_{};

  inline auto info_ptr() -> VkSubmitInfo* {
    return &info_;
  }

  inline auto info_ref() const -> const VkSubmitInfo& {
    return info_;
  }

 public:
  QueueSubmit();
  explicit QueueSubmit(VkQueue queue);
  ~QueueSubmit() = default;

  auto GetQueue() const -> const VkQueue& {
    return queue_;
  }

  auto GetHandle() const -> const VkSubmitInfo& {
    return info_ref();
  }

  auto WithWaitSemaphore(const VkSemaphore& rhs) -> QueueSubmit& {
    wait_semaphores_.push_back(rhs);
    return *this;
  }

  auto WithWaitStageMask(const VkPipelineStageFlags rhs) -> QueueSubmit& {
    wait_stages_.push_back(rhs);
    return *this;
  }

  auto WithCommandBuffer(const VkCommandBuffer& rhs) -> QueueSubmit& {
    command_buffers_.push_back(rhs);
    return *this;
  }

  auto WithCommandBuffers(const VkCommandBuffer* data, const uint64_t length) -> QueueSubmit& {
    ASSERT(data && length > 0);
    command_buffers_.insert(std::end(command_buffers_), data, data + length);
    return *this;
  }

  template <typename CommandBufferContainer>
  auto WithCommandBuffers(const CommandBufferContainer& data) -> QueueSubmit& {
    return WithCommandBuffers(data.data(), data.size());
  }

  auto WithSignalSemaphore(const VkSemaphore& rhs) -> QueueSubmit& {
    signal_semaphores_.push_back(rhs);
    return *this;
  }

  auto Submit(const VkFence& fence) -> Result;

  auto ToString() const -> std::string;
  operator VkSubmitInfo() const {
    return GetHandle();
  }
};

class QueuePresent {
 private:
  VkQueue queue_;
  VkPresentInfoKHR info_{};
  std::vector<VkSwapchainKHR> swapchains_{};
  std::vector<VkSemaphore> wait_semaphores_{};
  std::vector<uint32_t> images_{};

  inline auto info_ref() const -> const VkPresentInfoKHR& {
    return info_;
  }

  inline auto info_ptr() -> VkPresentInfoKHR* {
    return &info_;
  }

 public:
  QueuePresent();
  explicit QueuePresent(VkQueue queue);
  ~QueuePresent() = default;

  auto GetQueue() const -> const VkQueue& {
    return queue_;
  }

  auto GetInfo() const -> const VkPresentInfoKHR& {
    return info_ref();
  }

  auto WithImageIndex(const uint32_t rhs) -> QueuePresent& {
    images_.push_back(rhs);
    return *this;
  }

  auto WithSwap(const VkSwapchainKHR& rhs) -> QueuePresent& {
    swapchains_.push_back(rhs);
    return *this;
  }

  auto WithSwap(Swapchain* rhs) -> QueuePresent&;

  auto WithWaitSemaphore(const VkSemaphore& rhs) -> QueuePresent& {
    wait_semaphores_.push_back(rhs);
    return *this;
  }

  auto Present() -> Result;

  operator VkPresentInfoKHR() const {
    return info_ref();
  }
};
}  // namespace vk
}  // namespace prt

#endif  // PRT_VK_QUEUE_H
