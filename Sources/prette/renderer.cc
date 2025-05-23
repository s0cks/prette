#include "prette/renderer.h"

#include <sys/param.h>
#include <tweeny.h>
#include <vector>

#include "prette/common.h"
#include "prette/engine.h"
#include "prette/engine_event.h"
#include "prette/gfx.h"
#include "prette/gfx_driver.h"
#include "prette/gfx_driver_event.h"
#include "prette/gfx_vk.h"
#include "prette/glm.h"
#include "prette/pipeline.h"
#include "prette/relaxed_atomic.h"
#include "prette/render_pass.h"
#include "prette/renderer.h"
#include "prette/renderer_event.h"
#include "prette/renderer_state.h"
#include "prette/rx.h"
#include "prette/scene_renderer.h"
#include "prette/swapchain.h"
#include "prette/thread_local.h"
#include "prette/tick.h"
#include "prette/to_string.h"
#include "prette/vk.h"
#include "prette/vk_cmd_buffers.h"
#include "prette/window.h"
#include "prette/window_event.h"

namespace prt {
static RendererEventSubject events_{};
static ThreadLocal<Renderer> renderer_{};
static rx::subscription on_driver_init_{};
static rx::subscription on_driver_destroying_{};
static const std::vector<VkClearValue> kDefaultClearValues = {
    VkClearValue{
        .color = {0.1f, 0.1f, 0.1f, 1.0f},
    },
};

void Renderer::PublishEvent(RendererEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

auto OnRendererEvent() -> RendererEventObservable {
  return events_.get_observable();
}

Renderer::Renderer() :
  ticker_(&GetEngine()->GetLoop()) {
  InitRenderPass();
  on_window_resized_ = GetAppWindow()->OnWindowSizeEvent().subscribe([this](WindowSizeEvent* event) {
    ASSERT(event);
    OnWindowResized();
  });
}

Renderer::~Renderer() {
  delete command_buffers_;
  DestroySwap(false);
  StopTicker();
  on_window_resized_.unsubscribe();
}

void Renderer::OnWindowResized() {
  resized_ = true;
}

void Renderer::InitRenderPass() {
  // init render pass
  pass_ = Swapchain::CreateRenderPass();
  ASSERT_INITIALIZED(pass_);
}

void Renderer::InitPipeline() {
  pipeline_ = Swapchain::CreatePipeline();
  ASSERT_INITIALIZED(pipeline_);
}

void Renderer::InitSwap(const bool reinit) {
  ASSERT(!IsSwapchainInitialized());
  SwapchainInitializer::Init();
  if (!reinit) {
    InitPipeline();
    command_buffers_ = vk::CommandBufferPool<>::New();  // NOLINT(cppcoreguidelines-prefer-member-initializer)
    ASSERT(command_buffers_);
  }
  on_tick_ = StartTicker();
}

auto Renderer::StartTicker() -> rx::composite_subscription {
  ticker_.Start();
  return OnTick().subscribe([this](Tick tick) {
    DrawFrame(ticker_.GetCurrentTick(), ticker_.GetPreviousTick());
  });
}

void Renderer::DestroySwap(const bool reinit) {
  ASSERT(IsSwapchainInitialized());
  ticker_.Stop();
  SwapchainInitializer::DeInit();
  on_tick_.unsubscribe();
}

void Renderer::RenderSwap() {
  const auto swap = GetSwapchain();
  const auto frame = swap->GetCurrentFrame();
  vk::CommandBufferScope buffer(GetCommandBuffer(frame->GetImage()));
  vk::RenderPassScope render_pass(buffer, GetRenderPass(), *frame->GetFramebuffer(), kDefaultClearValues);
  GetPipeline()->Bind(buffer);
  const auto extent = glm::to_vec2(GetSwapchain()->GetExtent());
  vk::SetViewport(buffer, extent);
  vk::SetScissor(buffer, extent);
}

void Renderer::DrawFrame(const Tick& current, const Tick& previous) {
  SwapchainFrameScope frame{};
  RenderSwap();
  vk::RenderPassIterator iter(GetRenderPassList());
  while (iter.HasNext())
    iter.Next()->Execute();
}

void Renderer::Init() {
  ASSERT(IsRendererDestroyed());
  on_driver_init_ = OnDriverInitEvent().subscribe([](DriverInitEvent* event) {
    ASSERT(event);
    renderer_ = new Renderer();
    renderer_->InitSwap(false);
    renderer_->SetState(RendererState::kCreated);
    on_driver_init_.unsubscribe();
  });
  on_driver_destroying_ = OnDestroyingDriverEvent().subscribe([](DestroyingDriverEvent* event) {
    ASSERT(event);
    renderer_->SetState(RendererState::kDestroyed);
    renderer_.Release();
    on_driver_destroying_.unsubscribe();
  });
}

auto GetRenderer() -> Renderer* {
  return renderer_;
}

auto GetRendererState() -> RendererState {
  if (!renderer_.Has())
    return RendererState::kDestroyed;
  return renderer_->GetState();
}
}  // namespace prt