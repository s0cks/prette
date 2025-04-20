#ifndef PRT_SWAPCHAIN_H
#define PRT_SWAPCHAIN_H

#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/event.h"
#include "prette/gfx.h"
#include "prette/pipeline.h"
#include "prette/swapchain_event.h"
#include "prette/swapchain_frame.h"
#include "prette/swapchain_render_pass.h"
#include "prette/to_string.h"

namespace prt {
auto OnSwapChainEvent() -> SwapChainEventObservable;
#define DEFINE_ON_EVENT(Name)                                                     \
  static inline auto On##Name##Event()->Name##EventObservable {                   \
    return OnSwapChainEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
FOR_EACH_SWAPCHAIN_EVENT(DEFINE_ON_EVENT);
#undef DEFINE_ON_EVENT

class Driver;
class SwapChain {
  friend class Renderer;

 private:
  static void InitSwap(const Driver* driver, const bool is_reinit);
  static void DestroySwap(const Driver* driver, const bool is_reinit);
  static void ReInitSwap(const Driver* driver);
  static void InitWindowResizeListener();
  static void InitRenderPass(const Driver* driver);
  static void InitPipeline(const Driver* driver);

  static auto AcquireNextImage(const Driver* driver) -> bool;
  static void Present(const Driver* driver);
  static void Submit(const Driver* driver, const std::vector<VkCommandBuffer>& cmd_buffers);

  static void InitImages(const Driver* driver);

 public:
  static void Init();

  static auto GetSwapChain() -> VkSwapchainKHR const&;
  static auto GetImages() -> std::vector<VkImage> const&;
  static auto GetImageView(const uint32_t idx) -> VkImageView const&;
  static auto GetFormat() -> VkFormat const&;
  static auto GetExtent() -> VkExtent2D const&;
  static auto GetRenderPass() -> SwapChainRenderPass*;
  static auto GetGraphicsPipeline() -> GraphicsPipeline* const&;
  static auto GetCurrentFrame() -> SwapChainFrame*;
  static auto GetFramebuffer(const uint32_t idx) -> VkFramebuffer const&;

  static inline auto GetNumberOfImages() -> uint64_t {
    return GetImages().size();
  }

  static inline auto GetCurrentImage() -> uint32_t {
    return GetCurrentFrame()->GetImage();
  }
};
}  // namespace prt

#endif  // PRT_SWAPCHAIN_H
