#include "prette/renderer.h"

#include <imgui_impl_vulkan.h>
#include <vulkan/vulkan_core.h>

#include "prette/engine.h"
#include "prette/gfx.h"
#include "prette/gfx_driver.h"
#include "prette/gfx_vk.h"
#include "prette/gui.h"
#include "prette/lua.h"
#include "prette/pipeline.h"
#include "prette/relaxed_atomic.h"
#include "prette/scene_renderer.h"
#include "prette/shader.h"
#include "prette/swapchain.h"
#include "prette/to_string.h"
#include "prette/window.h"

namespace prt {
static RendererEventSubject events_{};
static RelaxedAtomic<bool> resized_(false);
static VkCommandPool command_pool_{};
static std::vector<VkCommandBuffer> command_buffers_{};

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

void Renderer::InitResizeListener() {
  const auto window = GetAppWindow();
  ASSERT(window);
  window->OnWindowSizeEvent().subscribe([](WindowSizeEvent* event) {
    ASSERT(event);
    resized_ = true;
  });
}

auto Renderer::GetCommandPool() -> VkCommandPool const& {
  return command_pool_;
}

void Renderer::DrawFrame(Driver* driver, const Tick& current, const Tick& previous) {
  ASSERT(driver);
  // pre frame
  SwapChain::AcquireNextImage(driver);
  Publish<PreFrameEvent>();

  // draw frame
  const auto& frame = SwapChain::GetCurrentFrame();
  std::vector<VkCommandBuffer> cmd_buffers{};
  {
    auto& buffer = command_buffers_.at(frame.frame);
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    CHECK_VK(FATAL, vkBeginCommandBuffer(buffer, &begin_info), "failed to begin vk command buffer");
    {
      VkRenderPassBeginInfo render_pass{};
      render_pass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      render_pass.renderPass = SwapChain::GetRenderPass();
      render_pass.framebuffer = SwapChain::GetFramebuffer(frame.image);
      render_pass.renderArea.offset = {0, 0};
      render_pass.renderArea.extent = SwapChain::GetExtent();
      std::array<VkClearValue, 2> clear_values = {
          {1.0f, 0.0f, 0.0f, 1.0f},
      };
      render_pass.clearValueCount = clear_values.size();
      render_pass.pClearValues = clear_values.data();

      vkCmdBeginRenderPass(buffer, &render_pass, VK_SUBPASS_CONTENTS_INLINE);
      vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, SwapChain::GetGraphicsPipeline().Get());

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

      vkCmdEndRenderPass(buffer);
    }
    CHECK_VK(FATAL, vkEndCommandBuffer(buffer), "failed to end vk command buffer");
    cmd_buffers.push_back(buffer);
  }

  SceneRenderer::Draw(frame.frame, frame.image, cmd_buffers);
  GuiRenderer::Draw(frame.frame, frame.image, cmd_buffers);

  // post frame
  SwapChain::Submit(driver, cmd_buffers);
  SwapChain::Present(driver);
  Publish<PostFrameEvent>();
}

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

void Renderer::InitLua(lua_State* L) {
  ASSERT(L);
  DLOG(INFO) << "initializing lua bindings....";
  lua_newtable(L);
  luaL_setfuncs(L, kRendererLib, 0);
  lua_setglobal(L, "Renderer");
}

void Renderer::InitCommandPool(const Driver* driver) {
  const auto indices = FindQueueFamilies(driver->GetPhysicalDevice(), driver->GetSurface());
  VkCommandPoolCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  create_info.queueFamilyIndex = indices.GetGraphicsFamily();
  CHECK_VK(FATAL, vkCreateCommandPool(driver->GetDevice(), &create_info, driver->GetAllocator(), &command_pool_),
           "failed to create vk command pool");

  command_buffers_.resize(SwapChain::GetNumberOfImages());
  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = command_pool_;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = command_buffers_.size();
  CHECK_VK(FATAL, vkAllocateCommandBuffers(driver->GetDevice(), &alloc_info, command_buffers_.data()),
           "failed to allocate vk command buffers");
}

void Renderer::Init() {
  OnSwapChainInitEvent().subscribe([](SwapChainInitEvent* event) {
    const auto driver = Driver::Get();
    ASSERT(driver);
    // renderer
    try {
      InitCommandPool(driver);
    } catch (const std::exception& exc) {
      LOG(ERROR) << "an exception has occured:";
      CrashReport report(CrashReportCause::New(std::current_exception()));
      report.Print(std::cerr);
      LOG(FATAL) << "";
    }
    InitResizeListener();
    Publish<RendererInitEvent>();
  });
  OnDestroyingDriverEvent().subscribe([](DestroyingDriverEvent* event) {
    Renderer::Destroy();
  });
}

void Renderer::Destroy() {
  const auto driver = Driver::Get();
  ASSERT(driver);
  vkFreeCommandBuffers(driver->GetDevice(), command_pool_, command_buffers_.size(), command_buffers_.data());
  vkDestroyCommandPool(driver->GetDevice(), command_pool_, driver->GetAllocator());
  Publish<RendererDestroyedEvent>();
}
}  // namespace prt