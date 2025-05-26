#ifndef PRT_SWAPCHAIN_H
#define PRT_SWAPCHAIN_H

#include <string>
#include <vector>

#include "prette/common.h"
#include "prette/framebuffer.h"
#include "prette/pipeline/pipeline.h"
#include "prette/relaxed_atomic.h"
#include "prette/rx.h"  // IWYU pragma: keep
#include "prette/swapchain/swapchain_event.h"
#include "prette/swapchain/swapchain_frame.h"
#include "prette/vk.h"

namespace prt {
auto GetSwapchainEventObservable() -> SwapchainEventObservable;

template <typename... ArgN>
static inline auto OnSwapchainEvent(ArgN... args) -> rx::composite_subscription {
  return GetSwapchainEventObservable().subscribe(args...);
}

#define DEFINE_ON_EVENT(Name)                                                                \
  static inline auto Get##Name##EventObservable()->Name##EventObservable {                   \
    return GetSwapchainEventObservable().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }                                                                                          \
  template <class... ArgN>                                                                   \
  static inline auto On##Name(ArgN... args)->rx::composite_subscription {                    \
    return Get##Name##EventObservable().subscribe(args...);                                  \
  }
FOR_EACH_SWAPCHAIN_EVENT(DEFINE_ON_EVENT);
#undef DEFINE_ON_EVENT

class Renderer;
class Swapchain : public vk::HandleTemplate<VkSwapchainKHR> {
  friend class Renderer;
  friend class SwapchainFrameScope;
  friend class SwapchainInitializer;

 public:
  static auto CreateRenderPass() -> vk::RenderPass*;
  static auto CreatePipeline(vk::DescriptorSet* dset) -> vk::RenderPipeline*;

 private:
  static void InitImages(const VkSwapchainKHR& swapchain, std::vector<VkImage>& images);
  static void PublishEvent(SwapchainEvent* event);

  template <class E, typename... Args>
  static inline void Publish(Args... args) {
    E event(args...);
    return PublishEvent(&event);
  }

#define DEFINE_PUBLISH(Name)                       \
  template <typename... Args>                      \
  static inline void Publish##Name(Args... args) { \
    return Publish<Name##Event>(args...);          \
  }
  FOR_EACH_SWAPCHAIN_EVENT(DEFINE_PUBLISH)
#undef DEFINE_PUBLISH

 private:
  VkExtent2D extent_;
  VkFormat format_;
  uint32_t num_images_ = 0;
  std::vector<VkImage> images_{};
  std::vector<vk::ImageView*> views_{};
  std::vector<vk::Framebuffer*> framebuffers_{};
  SwapchainFrameRingBuffer* frames_ = nullptr;
  RelaxedAtomic<bool> resized_{false};

  void SetResized(const bool rhs = true) {
    resized_ = rhs;
  }

  inline void ClearResized() {
    return SetResized(false);
  }

 public:
  explicit Swapchain(const VkSwapchainCreateInfoKHR* create_info);
  ~Swapchain();

  auto GetExtent() const -> const VkExtent2D& {
    return extent_;
  }

  auto GetFormat() const -> const VkFormat& {
    return format_;
  }

  auto GetNumberOfImages() const -> uint32_t {
    return images_.size();
  }

  auto IsResized() const -> bool {
    return (bool)resized_;
  }

  auto GetFramebuffer(const uint32_t idx) const -> vk::Framebuffer*;
  auto GetImage(const uint32_t idx) const -> const VkImage&;
  auto GetView(const uint32_t idx) const -> vk::ImageView*;
  auto ToString() const -> std::string override;

  auto GetViews() const -> const std::vector<vk::ImageView*>& {
    return views_;
  }

  auto GetCurrentFrame() const -> SwapchainFrame* {
    return frames_->GetCurrentFrame();
  }

  auto GetNextFrame() -> SwapchainFrame* {
    frames_->NextFrame();
    return GetCurrentFrame();
  }

  operator VkSwapchainKHR() const {
    return GetHandle();
  }
};

class SwapchainInitializer {
 private:
  bool init_;
  bool reinit_;

 public:
  explicit SwapchainInitializer(const bool reinit = false, const bool init = true) :
    reinit_(reinit),
    init_(init) {}
  ~SwapchainInitializer() = default;

  auto IsInit() const -> bool {
    return init_;
  }

  auto IsReinit() const -> bool {
    return reinit_;
  }

  void operator()();

 public:
  static inline void Init() {
    SwapchainInitializer init{};
    return init();
  }

  static inline void ReInit() {
    SwapchainInitializer init(true);
    return init();
  }

  static inline void DeInit() {
    SwapchainInitializer init(false, false);
    return init();
  }
};

auto IsSwapchainInitialized() -> bool;
auto GetSwapchain() -> Swapchain*;
}  // namespace prt

#endif  // PRT_SWAPCHAIN_H
