#ifndef PRT_SWAPCHAIN_FRAME_H
#define PRT_SWAPCHAIN_FRAME_H

#include "prette/common.h"
#include "prette/gfx.h"

namespace prt {
class SwapChainFrame {
  friend class SwapChain;
  DEFINE_DEFAULT_COPYABLE_TYPE(SwapChainFrame);

 private:
  uint32_t frame_ = 0;
  uint32_t image_ = 0;
  VkSemaphore available_ = VK_NULL_HANDLE;
  VkSemaphore finished_ = VK_NULL_HANDLE;
  VkFence fence_ = VK_NULL_HANDLE;

  void SetFrame(const uint32_t idx) {
    frame_ = idx;
  }

 public:
  SwapChainFrame() = default;
  SwapChainFrame(const uint32_t frame);
  ~SwapChainFrame();

  auto GetFrame() const -> uint32_t {
    return frame_;
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

  auto ToString() const -> std::string;
};
}  // namespace prt

#endif  // PRT_SWAPCHAIN_FRAME_H
