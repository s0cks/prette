#include "prette/renderer.h"

#include <sys/param.h>
#include <tweeny.h>

#include "prette/assertions.h"
#include "prette/camera_manager.h"
#include "prette/common.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/gfx.h"
#include "prette/gfx_driver.h"
#include "prette/gfx_driver_event.h"
#include "prette/gfx_vk.h"
#include "prette/gui/gui_system.h"
#include "prette/lua.h"
#include "prette/lua_event.h"
#include "prette/material/material_system.h"
#include "prette/relaxed_atomic.h"
#include "prette/render_pass/render_pass.h"
#include "prette/render_pass/scene_renderer.h"
#include "prette/renderer.h"
#include "prette/renderer_event.h"
#include "prette/renderer_lua_bindings.h"
#include "prette/rx.h"
#include "prette/swapchain/swapchain.h"
#include "prette/swapchain/swapchain_pass.h"
#include "prette/thread_local.h"
#include "prette/tick.h"
#include "prette/to_string.h"
#include "prette/vk.h"
#include "prette/vk_cmd_buffers.h"
#include "prette/window/window.h"
#include "prette/window/window_event.h"

namespace prt {
static RendererEventSubject events_{};
static ThreadLocal<Renderer> renderer_{};

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
  on_window_resized_ = GetAppWindow()->OnWindowSizeEvent().subscribe([this](WindowSizeEvent* event) {
    ASSERT(event);
    OnWindowResized();
  });
  on_driver_init_ = OnDriverInit([this](DriverInitEvent* event) {
    ASSERT(event);
    InitSwap(false);
  });
  on_destroying_driver_ = OnDestroyingDriver([this](DestroyingDriverEvent* event) {
    ASSERT(event);
    renderer_.Release();
  });
#ifdef PRT_ENABLE_LUA
  on_lua_init_ = OnLuaStateInitEvent().subscribe([this](LuaStateInitEvent* event) {
    RendererModule::Init(GetLua()->GetState());
  });
#endif  // PRT_ENABLE_LUA
}

Renderer::~Renderer() {
  delete command_buffers_;
  DestroySwap(false);
  StopTicker();
  on_window_resized_.unsubscribe();
  on_driver_init_.unsubscribe();
  on_destroying_driver_.unsubscribe();
#ifdef PRT_ENABLE_LUA
  on_lua_init_.unsubscribe();
#endif  // PRT_ENABLE_LUA
}

void Renderer::OnWindowResized() {
  resized_ = true;
}

void Renderer::InitRenderPasses() {
  DVLOG(1) << "initializing render passes.....";
  PublishInitRenderPassesEvent();
}

void Renderer::InitPipelines() {
  DVLOG(1) << "initializing pipelines.....";
  PublishInitGraphicsPipelinesEvent();
}

void Renderer::InitPipelineLayouts() {
  DVLOG(1) << "initializing pipeline layouts....";
  PublishInitPipelineLayoutsEvent();
}

void Renderer::InitBuffers() {
  DVLOG(1) << "initializing buffers.....";
  command_buffers_ = vk::CommandBufferPool<>::New();  // NOLINT(cppcoreguidelines-prefer-member-initializer)
  ASSERT(command_buffers_);
  PublishInitBuffersEvent();
}

void Renderer::InitDescriptorSets() {
  DVLOG(1) << "initializing descriptor sets....";
  PublishInitDescriptorSetsEvent();
}

void Renderer::InitMeshes() {
  DVLOG(1) << "initializing meshes.....";
  PublishInitMeshesEvent();
}

void Renderer::InitTextures() {
  DVLOG(1) << "initializing textures.....";
  PublishInitTexturesEvent();
}

void Renderer::InitMaterials() {
  DVLOG(1) << "initializing materials.....";
  PublishInitMaterialsEvent();
}

void Renderer::InitSwap(const bool reinit) {
  ASSERT(!IsSwapchainInitialized());
  pass_ = vk::SwapchainRenderPass::New();
  ASSERT_INITIALIZED(pass_);
  SwapchainInitializer::Init();
  if (!reinit) {
    InitDescriptorSets();
    InitPipelineLayouts();
    InitRenderPasses();
    InitPipelines();
    InitTextures();
    InitMaterials();
    InitBuffers();
    InitMeshes();
  }
  on_tick_ = StartTicker();
}

void Renderer::UpdateDescriptors() {}

auto Renderer::StartTicker() -> rx::composite_subscription {
  ticker_.Start();
  PublishRendererStartedEvent();
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

void Renderer::DrawFrame(const Tick& current, const Tick& previous) {
  SwapchainFrameScope frame{};
  GetSceneRenderer()->GetScenePass()->Execute();
  // GuiSystem::GetSystem()->GetGuiPass()->Execute();
}

void Renderer::Init() {
  ASSERT(!IsRendererInitialized());
  gfx::Init();
  Window::Init();
  Driver::Init();
  InitCameraManager();
  MaterialSystem::InitSystem();
  GuiSystem::InitSystem();
  InitSceneRenderer();

  renderer_ = new Renderer();
  ASSERT(IsRendererInitialized());
}

auto GetRenderer() -> Renderer* {
  ASSERT(IsRendererInitialized());
  return renderer_;
}

auto IsRendererInitialized() -> bool {
  return renderer_.Get() != nullptr;
}
}  // namespace prt