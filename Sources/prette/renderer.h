#ifndef PRT_RENDERER_H
#define PRT_RENDERER_H

#include "prette/common.h"
#include "prette/engine.h"
#include "prette/lua.h"
#include "prette/platform.h"
#include "prette/relaxed_atomic.h"
#include "prette/render_pass.h"
#include "prette/renderer_event.h"
#include "prette/renderer_state.h"
#include "prette/rx.h"
#include "prette/tick.h"
#include "prette/ticker.h"
#include "prette/vk.h"
#include "prette/vk_cmd_buffers.h"

namespace prt {
auto OnRendererEvent() -> RendererEventObservable;
#define DEFINE_ON_EVENT(Name)                                                    \
  static inline auto On##Name##Event()->Name##EventObservable {                  \
    return OnRendererEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
FOR_EACH_RENDERER_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

static inline auto OnRendererState(const RendererState rhs) -> RendererStateEventObservable {
  // clang-format off
  return OnRendererEvent()
    .filter(RendererStateEvent::FilterByState(rhs))
    .map(RendererStateEvent::Cast);
  // clang-format on
}

#define DEFINE_ON_STATE(Name)                                           \
  static inline auto OnRenderer##Name()->RendererStateEventObservable { \
    return OnRendererState(RendererState::k##Name);                     \
  }
FOR_EACH_RENDERER_STATE(DEFINE_ON_STATE)
#undef DEFINE_ON_STATE

class Renderer {
  friend class LuaState;
  friend class Swapchain;
  friend class SwapchainFrameScope;

 private:
#ifdef PRETTE_ENABLE_LUA
  static void InitLua(lua_State* L);
#endif  // PRETTE_ENABLE_LUA
  static void PublishEvent(RendererEvent* event);

  template <class E, typename... Args>
  static inline void Publish(Args... args) {
    E event(args...);
    return PublishEvent(&event);
  }

 private:
  RelaxedAtomic<RendererState> state_;
  RateLimitedTicker<kTargetTickRate> ticker_;
  vk::RenderPass* pass_ = nullptr;
  vk::RenderPipeline* pipeline_ = nullptr;
  vk::RenderPass* render_passes_ = nullptr;
  vk::CommandBufferPool<>* command_buffers_ = nullptr;
  RelaxedAtomic<bool> resized_{false};
  rx::subscription on_tick_{};
  rx::subscription on_window_resized_{};

  void InitPipeline();
  void InitRenderPass();
  void InitSwap(const bool reinit);
  void DestroySwap(const bool reinit);
  void RenderSwap();
  void OnWindowResized();
  auto StartTicker() -> rx::composite_subscription;

  inline void StopTicker() {
    on_tick_.unsubscribe();
    return ticker_.Stop();
  }

  void SetState(const RendererState rhs) {
    state_ = rhs;
    Publish<RendererStateEvent>(rhs);
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
auto GetRendererState() -> RendererState;

#define DEFINE_STATE_CHECK(Name)                         \
  static inline auto IsRenderer##Name()->bool {          \
    return GetRendererState() == RendererState::k##Name; \
  }
FOR_EACH_RENDERER_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK
}  // namespace prt

#endif  // PRT_RENDERER_H
