#ifndef PRT_SWAPCHAIN_FRAME_H
#define PRT_SWAPCHAIN_FRAME_H

#include <cstdint>
#include <string>
#include <vector>

#include "prette/common.h"
#include "prette/relaxed_atomic.h"
#include "prette/vk.h"

namespace prt {
class Swapchain;
class SwapchainFrame {
  friend class SwapchainRenderer;
  DEFINE_DEFAULT_COPYABLE_TYPE(SwapchainFrame);

 public:
  using CommandBufferList = std::vector<VkCommandBuffer>;

 private:
  Swapchain* owner_;
  uint32_t frame_ = 0;
  uint32_t image_ = 0;
  VkSemaphore available_ = VK_NULL_HANDLE;
  VkSemaphore finished_ = VK_NULL_HANDLE;
  VkFence fence_ = VK_NULL_HANDLE;
  CommandBufferList buffers_{};

 public:
  SwapchainFrame(Swapchain* owner, const uint32_t frame);
  ~SwapchainFrame();

  auto GetOwner() const -> Swapchain* {
    return owner_;
  }

  auto GetView() const -> vk::ImageView*;
  auto GetFramebuffer() const -> vk::Framebuffer*;

  auto GetComamndBuffers() const -> const CommandBufferList& {
    return buffers_;
  }

  auto GetFrame() const -> uint32_t {
    return frame_;
  }

  void SetFrame(const uint32_t idx) {
    frame_ = idx;
  }

  void ClearBuffers() {
    buffers_.clear();
  }

  auto GetImage() const -> uint32_t {
    return image_;
  }

  auto GetAvailableSemaphore() const -> const VkSemaphore& {
    return available_;
  }

  auto GetFinishedSemaphore() const -> const VkSemaphore& {
    return finished_;
  }

  auto GetFence() const -> const VkFence& {
    return fence_;
  }

  void Append(VkCommandBuffer& buffer) {
    buffers_.push_back(buffer);
  }

  auto GetNumberOfCommandBuffers() const -> uint64_t {
    return buffers_.size();
  }

  inline auto HasCommandBuffers() const -> bool {
    return GetNumberOfCommandBuffers() > 0;
  }

  auto AcquireNextImage(const uint64_t timeout = UINT64_MAX, const bool waitAll = VK_TRUE) -> vk::Result;
  auto ToString() const -> std::string;
};

class SwapchainFrameRingBuffer {
 private:
  Swapchain* owner_;
  std::vector<SwapchainFrame*> frames_{};
  RelaxedAtomic<uint32_t> current_{0};

 public:
  SwapchainFrameRingBuffer(Swapchain* owner, const uint64_t num_frames);
  ~SwapchainFrameRingBuffer();

  auto GetOwner() const -> Swapchain* {
    return owner_;
  }

  auto GetFrame(const uint32_t idx) const -> SwapchainFrame* {
    return frames_.at(idx);
  }

  auto GetCurrentFrame() const -> SwapchainFrame* {
    return frames_.at((uint32_t)current_);
  }

  void NextFrame() {
    current_ = (((uint32_t)current_) + 1) % (frames_.size() - 1);
    frames_.at((uint32_t)current_)->ClearBuffers();
  }

  void Reset() {
    for (auto& frame : frames_) {
      frame->ClearBuffers();
    }
  }
};

class SwapchainFrameScope {
 private:
  SwapchainFrame* frame_ = nullptr;
  bool submit_ = true;
  bool present_ = true;

  auto AcquireNextImage(const uint64_t timeout = UINT64_MAX, const bool waitAll = VK_TRUE) -> SwapchainFrame*;
  void Submit();
  void Present();

 public:
  SwapchainFrameScope();
  ~SwapchainFrameScope();

  auto ShouldSubmit() const -> bool {
    return submit_;
  }

  void CancelSubmit() {
    submit_ = false;
  }

  auto ShouldPresent() const -> bool {
    return present_;
  }

  void CancelPresent() {
    present_ = false;
  }

  inline void Cancel() {
    submit_ = present_ = false;
  }

  auto GetCurrent() const -> SwapchainFrame* {
    return frame_;
  }

  auto operator->() const -> SwapchainFrame* {
    return GetCurrent();
  }

  operator SwapchainFrame*() const {
    return GetCurrent();
  }
};

}  // namespace prt

#endif  // PRT_SWAPCHAIN_FRAME_H
