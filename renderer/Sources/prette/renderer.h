#ifndef PRT_RENDERER_H
#define PRT_RENDERER_H

#include "prette/common.h"
#include "prette/engine/engine.h"
#include "prette/platform.h"
#include "prette/relaxed_atomic.h"
#include "prette/render_pass/render_pass.h"
#include "prette/renderer_event.h"
#include "prette/renderer_state.h"
#include "prette/rx.h"
#include "prette/swapchain/swapchain_pass.h"
#include "prette/tick.h"
#include "prette/ticker.h"
#include "prette/vk.h"
#include "prette/vk_cmd_buffers.h"

namespace prt {
auto OnRendererEvent() -> RendererEventObservable;
#define DEFINE_ON_EVENT(Name)                                                    \
  static inline auto On##Name##Event()->Name##EventObservable {                  \
    return OnRendererEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }                                                                              \
  template <class... ArgN>                                                       \
  static inline auto On##Name(ArgN... args)->rx::composite_subscription {        \
    return On##Name##Event().subscribe(args...);                                 \
  }
FOR_EACH_RENDERER_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

static inline auto GetNonFrameRendererEventObservable() -> RendererEventObservable {
  return OnRendererEvent().filter([](RendererEvent* event) {
    return event && !event->IsFrameEvent();
  });
}

class Renderer {
  friend class Swapchain;
  friend class SwapchainFrameScope;

 private:
  static auto CreateDescriptors() -> vk::DescriptorSet*;
  static void PublishEvent(RendererEvent* event);

  template <class E, typename... Args>
  static inline void Publish(Args... args) {
    E event(args...);
    return PublishEvent(&event);
  }

#define DEFINE_PUBLISH(Name)                              \
  template <typename... Args>                             \
  static inline void Publish##Name##Event(Args... args) { \
    return Publish<Name##Event>(args...);                 \
  }
  FOR_EACH_RENDERER_EVENT(DEFINE_PUBLISH)
#undef DEFINE_PUBLISH

 private:
  RelaxedAtomic<RendererState> state_;
  RateLimitedTicker<kTargetTickRate> ticker_;
  vk::SwapchainRenderPass* pass_ = nullptr;
  vk::RenderPipeline* pipeline_ = nullptr;
  vk::RenderPass* render_passes_ = nullptr;
  vk::CommandBufferPool<>* command_buffers_ = nullptr;
  vk::DescriptorSet* descriptors_ = nullptr;
  RelaxedAtomic<bool> resized_{false};
  rx::subscription on_tick_{};
  rx::subscription on_window_resized_{};
  rx::subscription on_lua_init_{};
  rx::subscription on_driver_init_{};
  rx::subscription on_destroying_driver_{};

  void InitDescriptorSets();
  void InitPipelines();
  void InitRenderPasses();
  void InitPipelineLayouts();
  void InitBuffers();

  void InitSwap(const bool reinit);
  void DestroySwap(const bool reinit);
  void RenderSwap();
  void OnWindowResized();
  auto StartTicker() -> rx::composite_subscription;
  void UpdateDescriptors();

  inline void SetState(const RendererState rhs) {
    state_ = rhs;
  }

  inline void StopTicker() {
    on_tick_.unsubscribe();
    return ticker_.Stop();
  }

  auto GetRenderPass() const -> vk::RenderPass* {
    return pass_;
  }

  auto GetPipeline() const -> vk::RenderPipeline* {
    return pipeline_;
  }

  auto GetCommandBuffer(const uint32_t idx) -> VkCommandBuffer* {
    return &command_buffers_->At(idx);
  }

  inline auto OnTick() -> rx::observable<Tick> {
    return ticker_.OnTick();
  }

 public:
  Renderer();
  ~Renderer();

  auto GetState() const -> RendererState {
    return (RendererState)state_;
  }

#define DEFINE_STATE_CHECK(Name)                 \
  inline auto Is##Name() const->bool {           \
    return GetState() == RendererState::k##Name; \
  }
  FOR_EACH_RENDERER_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK

  void DrawFrame(const Tick& current, const Tick& previous);

  auto GetRenderPassList() const -> vk::RenderPass* {
    return render_passes_;
  }

  inline auto HasRenderPasses() const -> bool {
    return GetRenderPassList() != nullptr;
  }

  void AddRenderPass(vk::RenderPass* rhs) {
    ASSERT(rhs);
    Append(&render_passes_, rhs);
  }

  void RemoveRenderPass(vk::RenderPass* rhs) {
    ASSERT(rhs);
    Remove(&render_passes_, rhs);
  }

 public:
  static void Init();
};

auto GetRenderer() -> Renderer*;
auto IsRendererInitialized() -> bool;
}  // namespace prt

#endif  // PRT_RENDERER_H
