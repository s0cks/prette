#include "prette/renderer.h"

#include <easing.h>
#include <imgui_impl_vulkan.h>
#include <mach/clock_types.h>
#include <tweeny.h>
#include <vulkan/vulkan_core.h>

#include <glm/ext/matrix_clip_space.hpp>

#include "prette/camera.h"
#include "prette/engine.h"
#include "prette/gfx.h"
#include "prette/gfx_driver.h"
#include "prette/gfx_vk.h"
#include "prette/gui.h"
#include "prette/lua.h"
#include "prette/lua_event.h"
#include "prette/pipeline.h"
#include "prette/relaxed_atomic.h"
#include "prette/scene_renderer.h"
#include "prette/swapchain.h"
#include "prette/to_string.h"
#include "prette/window.h"

namespace prt {
static RendererEventSubject events_{};
static std::array<VkCommandBuffer, MAX_NUMBER_OF_FRAMES_IN_FLIGHT> command_buffers_{};

static auto kTween = tweeny::from(0.0f).to(360.0f).during(kTargetTicksPerSecond * 100).via(tweeny::easing::circularInOut);

static inline void PublishRendererEvent(RendererEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

template <class E, typename... Args>
static inline void Publish(Args... args) {
  E event(args...);
  return PublishRendererEvent(&event);
}

auto OnRendererEvent() -> RendererEventObservable {
  return events_.get_observable();
}

auto RendererInitEvent::ToString() const -> std::string {
  ToStringHelper<RendererInitEvent> helper{};
  return helper;
}

auto PreFrameEvent::ToString() const -> std::string {
  ToStringHelper<PreFrameEvent> helper{};
  return helper;
}

auto PostFrameEvent::ToString() const -> std::string {
  ToStringHelper<PostFrameEvent> helper{};
  return helper;
}

auto RendererDestroyedEvent::ToString() const -> std::string {
  ToStringHelper<RendererDestroyedEvent> helper{};
  return helper;
}

void Renderer::InitCommandBuffers(const Driver* driver) {
  ASSERT(driver);
  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = driver->GetCommandPool();
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = command_buffers_.size();
  CHECK_VK(FATAL, vkAllocateCommandBuffers(driver->GetDevice(), &alloc_info, command_buffers_.data()),
           "failed to allocate vk command buffers");
}

void Renderer::DrawFrame(Driver* driver, const Tick& current, const Tick& previous) {
  ASSERT(driver);
  // clang-format off
  static const std::vector<VkClearValue> kClearValues = {
    VkClearValue{.color = {0.0f, 0.0f, 0.0f, 1.0f }}
  };
  // clang-format on
  // pre frame
  SwapChain::AcquireNextImage(driver);
  Publish<PreFrameEvent>();

  // draw frame
  const auto frame = SwapChain::GetCurrentFrame();
  std::vector<VkCommandBuffer> cmd_buffers{};
  {
    CommandBufferScope buffer(command_buffers_.at(frame->GetFrame()));
    {
      RenderPassScope render_pass(buffer, SwapChain::GetRenderPass()->Get(), SwapChain::GetFramebuffer(frame->GetImage()),
                                  kClearValues);
      vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, SwapChain::GetGraphicsPipeline()->GetPipeline());

      VkViewport viewport{};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = (float)SwapChain::GetExtent().width;
      viewport.height = (float)SwapChain::GetExtent().height;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(buffer, 0, 1, &viewport);

      VkRect2D scissor{};
      scissor.offset = {0, 0};
      scissor.extent = SwapChain::GetExtent();
      vkCmdSetScissor(buffer, 0, 1, &scissor);
    }
    cmd_buffers.push_back(buffer);
  }

  {
    // update camera
    const auto camera = Camera::Get();
    ASSERT(camera);
    auto data = camera->data();
    // data.model = glm::mat4(1.0f);
    // data.model = glm::translate(data.model, glm::vec3(0.0f, 0.0f, -1.0f));
    camera->Update();
    // const auto rot = kTween.step(1);
    // data.model = glm::rotate(data.model, rot, glm::vec3(0.0f, 1.0f, 0.0f));
    data.projection[1][1] *= -1;
    const auto buffer = SceneRenderer::GetCameraBuffer(frame->GetFrame());
    ASSERT(buffer);
    buffer->CopyFromBytes(&data, sizeof(CameraData));
  }

  SceneRenderer::Draw(frame, cmd_buffers);
  GuiRenderer::Draw(frame, cmd_buffers);

  // post frame
  SwapChain::Submit(driver, cmd_buffers);
  SwapChain::Present(driver);
  Publish<PostFrameEvent>();
}

class RendererModule : LuaModule {
  friend class Renderer;
  friend class LuaState;

 private:
  static void Init(lua_State* L);
};

#define LUA_RENDERER_F(Name) LUA_F(renderer_##Name)

LUA_RENDERER_F(onEvent) {
  OnRendererEvent().subscribe(CreateSubscriber<RendererEvent>(L));
  return 0;
}

#define DEFINE_ON_EVENT_FUNC(Name)                                 \
  LUA_RENDERER_F(on##Name##Event) {                                \
    On##Name##Event().subscribe(CreateSubscriber<Name##Event>(L)); \
    return 0;                                                      \
  }
FOR_EACH_RENDERER_EVENT(DEFINE_ON_EVENT_FUNC);
#undef DEFINE_ON_EVENT_FUNC

#undef LUA_RENDERER_F

// clang-format off
// NOLINTNEXTLINE
static const struct luaL_Reg kRendererLib[] = {
#define LUA_RENDERER_F(Name) \
  { .name = #Name, .func = &lua_renderer_##Name }

  LUA_RENDERER_F(onEvent),
#define DEFINE_ON_EVENT(Name) \
  LUA_RENDERER_F(on##Name##Event),
FOR_EACH_RENDERER_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
#undef LUA_RENDERER_F
};
// clang-format on

void RendererModule::Init(lua_State* L) {
  InitModule(L, "renderer", "Renderer", kRendererLib);
}

void Renderer::InitLua(lua_State* L) {
  return RendererModule::Init(L);
}

void Renderer::Init() {
  OnSwapChainInitEvent().subscribe([](SwapChainInitEvent* event) {
    const auto driver = Driver::Get();
    ASSERT(driver);
    InitCommandBuffers(driver);
    Publish<RendererInitEvent>();
  });
  OnDestroyingDriverEvent().subscribe([](DestroyingDriverEvent* event) {
    Renderer::Destroy();
  });
}

void Renderer::Destroy() {
  const auto driver = Driver::Get();
  ASSERT(driver);
  driver->ReleaseCommandBuffers(command_buffers_);
  Publish<RendererDestroyedEvent>();
}
}  // namespace prt