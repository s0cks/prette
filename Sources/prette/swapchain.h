#ifndef PRT_SWAPCHAIN_H
#define PRT_SWAPCHAIN_H

#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/event.h"
#include "prette/gfx.h"
#include "prette/pipeline.h"

namespace prt {
struct SwapChainFrame {
  friend class SwapChain;

  uint32_t frame{};
  uint32_t image{};
  VkSemaphore available{};
  VkSemaphore finished{};
  VkFence fence{};

  SwapChainFrame() = default;
  SwapChainFrame(const uint32_t frame, const Driver* driver);
  ~SwapChainFrame() = default;

  DEFINE_DEFAULT_COPYABLE_TYPE(SwapChainFrame);

 private:
  void Destroy(const Driver* driver);
};

#define FOR_EACH_SWAPCHAIN_EVENT(V) \
  V(SwapChainInit)                  \
  V(SwapChainDestroyed)

class SwapChainEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_SWAPCHAIN_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

class SwapChainEvent : public Event {
 private:
  bool reinit_;

 public:
  explicit SwapChainEvent(const bool reinit) :
    Event(),
    reinit_(reinit) {}
  ~SwapChainEvent() override = default;

  auto IsReinit() const -> bool {
    return reinit_;
  }

  DEFINE_EVENT_PROTOTYPE(SwapChain, FOR_EACH_SWAPCHAIN_EVENT);
};

class SwapChainInitEvent : public SwapChainEvent {
 public:
  explicit SwapChainInitEvent(const bool reinit) :
    SwapChainEvent(reinit) {}
  ~SwapChainInitEvent() override = default;
  DECLARE_EVENT_TYPE(SwapChainEvent, SwapChainInit);
};

class SwapChainDestroyedEvent : public SwapChainEvent {
 public:
  explicit SwapChainDestroyedEvent(const bool reinit) :
    SwapChainEvent(reinit) {}
  ~SwapChainDestroyedEvent() override = default;
  DECLARE_EVENT_TYPE(SwapChainEvent, SwapChainDestroyed);
};

DEFINE_EVENT_SUBJECT(SwapChain);
DEFINE_EVENT_OBSERVABLE(SwapChain);
FOR_EACH_SWAPCHAIN_EVENT(DEFINE_EVENT_OBSERVABLE);

auto OnSwapChainEvent() -> SwapChainEventObservable;

#define DEFINE_ON_EVENT(Name)                                                     \
  static inline auto On##Name##Event()->Name##EventObservable {                   \
    return OnSwapChainEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
FOR_EACH_SWAPCHAIN_EVENT(DEFINE_ON_EVENT);
#undef DEFINE_ON_EVENT

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR surface_capabilities{};
  std::vector<VkSurfaceFormatKHR> surface_formats{};
  std::vector<VkPresentModeKHR> present_modes{};

  inline auto HasSurfaceFormats() const -> bool {
    return !surface_formats.empty();
  }

  inline auto HasPresentModes() const -> bool {
    return !present_modes.empty();
  }

  auto FindSurfaceFormat(const std::function<bool(const VkSurfaceFormatKHR&)>& filter) const -> const VkSurfaceFormatKHR& {
    const auto pos = std::ranges::find_if(surface_formats, filter);
    if (pos != std::end(surface_formats))
      return (*pos);
    DLOG(WARNING) << "failed to find valid surface format for swap chain.";
    return surface_formats[0];
  }

  auto FindPresentMode(const std::function<bool(const VkPresentModeKHR&)>& filter) const -> VkPresentModeKHR {
    const auto pos = std::ranges::find_if(present_modes, filter);
    if (pos != std::end(present_modes))
      return (*pos);
    DLOG(WARNING) << "failed to find valid present mode for swap chain.";
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  auto GetMaxImageCount() const -> uint32_t {
    return surface_capabilities.maxImageCount;
  }

  inline auto HasMaxImage() const -> bool {
    return GetMaxImageCount() > 0;
  }

  void ClampImageCount(uint32_t& image_count) const {
    if (HasMaxImage() && image_count > GetMaxImageCount())
      image_count = GetMaxImageCount();
  }

  auto GetExtent() const -> VkExtent2D;

  operator bool() const {
    return HasSurfaceFormats() && HasPresentModes();
  }
};

auto QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> SwapChainSupportDetails;

static inline auto HasSwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> bool {
  return QuerySwapChainSupport(device, surface);
}

class Driver;
class SwapChain {
  friend class Renderer;

 private:
  static void InitSwap(const Driver* driver, const bool is_reinit);
  static void DestroySwap(const Driver* driver, const bool is_reinit);
  static void ReInitSwap(const Driver* driver);
  static void InitWindowResizeListener();
  static void InitRenderPass(const Driver* driver);
  static void InitDescriptorSets(const Driver* driver, const uint64_t num_sets);
  static void InitPipeline(const Driver* driver);

  static auto AcquireNextImage(const Driver* driver) -> bool;
  static void Present(const Driver* driver);
  static void Submit(const Driver* driver, const std::vector<VkCommandBuffer>& cmd_buffers);

  static void InitImages(const Driver* driver);

 public:
  static void Init();

  static auto GetSwapChain() -> VkSwapchainKHR const&;
  static auto GetImages() -> std::vector<VkImage> const&;
  static auto GetImageViews() -> std::vector<VkImageView> const&;
  static auto GetImageView(const uint32_t idx) -> VkImageView const&;
  static auto GetFormat() -> VkFormat const&;
  static auto GetExtent() -> VkExtent2D const&;
  static auto GetRenderPass() -> VkRenderPass const&;
  static auto GetGraphicsPipeline() -> GraphicsPipeline const&;
  static auto GetCurrentFrame() -> SwapChainFrame const&;
  static auto GetFramebuffer(const uint32_t idx) -> VkFramebuffer const&;

  static inline auto GetNumberOfImages() -> uint64_t {
    return GetImages().size();
  }

  static inline auto GetCurrentImage() -> uint32_t {
    return GetCurrentFrame().image;
  }
};
}  // namespace prt

#endif  // PRT_SWAPCHAIN_H
