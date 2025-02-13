#include "prette/renderer.h"

#include <imgui_impl_vulkan.h>
#include <vulkan/vulkan_core.h>

#include "prette/engine.h"
#include "prette/gfx.h"
#include "prette/gfx_vk.h"
#include "prette/gui.h"
#include "prette/lua.h"
#include "prette/relaxed_atomic.h"
#include "prette/scene_renderer.h"
#include "prette/shader.h"
#include "prette/to_string.h"
#include "prette/window.h"

namespace prt {
template <const int NumFrames>
struct SwapChainFence {
  std::array<VkSemaphore, NumFrames> available{};
  std::array<VkSemaphore, NumFrames> finished{};
  std::array<VkFence, NumFrames> fences{};

  auto FenceAt(const uint32_t frame) const -> VkFence const& {
    return fences.at(frame);
  }

  auto AvailableAt(const uint32_t frame) const -> VkSemaphore const& {
    return available.at(frame);
  }

  auto FinishedAt(const uint32_t frame) const -> VkSemaphore const& {
    return finished.at(frame);
  }

  void Init(const Driver* driver) {
    ASSERT(driver);
    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (auto idx = 0; idx < MAX_NUMBER_OF_FRAMES_IN_FLIGHT; idx++) {
      CHECK_VK(FATAL, vkCreateSemaphore(driver->GetDevice(), &semaphore_info, driver->GetAllocator(), &available.at(idx)),
               "failed to create available semaphore");
      CHECK_VK(FATAL, vkCreateSemaphore(driver->GetDevice(), &semaphore_info, driver->GetAllocator(), &finished.at(idx)),
               "failed to create finished semaphore");
      CHECK_VK(FATAL, vkCreateFence(driver->GetDevice(), &fence_info, driver->GetAllocator(), &fences.at(idx)),
               "failed to create fence");
    }
  }

  void Destroy(const Driver* driver) {
    ASSERT(driver);
    for (auto idx = 0; idx < MAX_NUMBER_OF_FRAMES_IN_FLIGHT; idx++) {
      vkDestroySemaphore(driver->GetDevice(), finished.at(idx), driver->GetAllocator());
      vkDestroySemaphore(driver->GetDevice(), available.at(idx), driver->GetAllocator());
      vkDestroyFence(driver->GetDevice(), fences.at(idx), driver->GetAllocator());
    }
  }
};

static RendererEventSubject events_{};

static SwapChainFence<MAX_NUMBER_OF_FRAMES_IN_FLIGHT> fence_{};

static RelaxedAtomic<uint32_t> current_frame_(0);
static RelaxedAtomic<bool> resized_(false);
static VkSwapchainKHR chain_{};
static std::vector<VkImage> images_{};
static std::vector<VkImageView> views_{};
static VkFormat format_{};
static VkExtent2D extent_{};
static VkCommandPool command_pool_{};

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

auto SwapChainInitEvent::ToString() const -> std::string {
  ToStringHelper<SwapChainInitEvent> helper{};
  helper.AddFieldRef("reinit", IsReinit());
  return helper;
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

auto SwapChainDestroyedEvent::ToString() const -> std::string {
  ToStringHelper<SwapChainDestroyedEvent> helper{};
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

auto Renderer::GetCurrentFrame() -> uint32_t {
  return (uint32_t)current_frame_;
}

auto Renderer::GetFormat() -> VkFormat const& {
  return format_;
}

auto Renderer::GetImageViews() -> std::vector<VkImageView> const& {
  return views_;
}

auto Renderer::GetImageView(const uint32_t idx) -> VkImageView const& {
  return views_[idx];
}

auto QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> SwapChainSupportDetails {
  SwapChainSupportDetails details{};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.surface_capabilities);

  uint32_t num_formats = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &num_formats, nullptr);
  if (num_formats > 0) {
    details.surface_formats.resize(num_formats);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &num_formats, &details.surface_formats[0]);
  }

  uint32_t num_present_modes = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &num_present_modes, nullptr);
  if (num_present_modes > 0) {
    details.present_modes.resize(num_present_modes);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &num_present_modes, &details.present_modes[0]);
  }
  return details;
}

auto SwapChainSupportDetails::GetExtent() const -> VkExtent2D {
  if (surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return surface_capabilities.currentExtent;
  }

  const auto window = GetAppWindow();
  ASSERT(window);
  VkExtent2D actual = window->GetFramebufferSize();
  return ClampExtent(actual, surface_capabilities);
}

static inline auto IsValidSwapChainSurfaceFormat(const VkSurfaceFormatKHR& format) -> bool {
  return format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
}

static inline auto IsValidSwapChainPresentMode(const VkPresentModeKHR& mode) -> bool {
  return mode == VK_PRESENT_MODE_MAILBOX_KHR;
}

auto Renderer::GetSwapChain() -> VkSwapchainKHR const& {
  return chain_;
}

auto Renderer::GetExtent() -> VkExtent2D const& {
  return extent_;
}

void Renderer::InitSwapChain(const Driver* driver, const bool reinit) {
  ASSERT(driver);
  const auto support = QuerySwapChainSupport(driver->GetPhysicalDevice(), driver->GetSurface());
  const auto surface_format = support.FindSurfaceFormat(&IsValidSwapChainSurfaceFormat);
  const auto present_mode = support.FindPresentMode(&IsValidSwapChainPresentMode);
  const auto extent = support.GetExtent();

  uint32_t image_count = support.surface_capabilities.minImageCount + 1;
  support.ClampImageCount(image_count);

  VkSwapchainCreateInfoKHR create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = driver->GetSurface();
  create_info.minImageCount = image_count;
  create_info.imageFormat = surface_format.format;
  create_info.imageColorSpace = surface_format.colorSpace;
  create_info.imageExtent = extent;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  const auto indices = FindQueueFamilies(driver->GetPhysicalDevice(), driver->GetSurface());
  const std::array<uint32_t, 2> families = {
      indices.GetGraphicsFamily(),
      indices.GetPresentFamily(),
  };
  if (indices.GetGraphicsFamily() != indices.GetPresentFamily()) {
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = &families[0];
  } else {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  create_info.preTransform = support.surface_capabilities.currentTransform;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  create_info.presentMode = present_mode;
  create_info.clipped = VK_TRUE;
  create_info.oldSwapchain = VK_NULL_HANDLE;
  CHECK_VK(FATAL, vkCreateSwapchainKHR(driver->GetDevice(), &create_info, nullptr, &chain_), "failed to create vk swap chain");
  vkGetSwapchainImagesKHR(driver->GetDevice(), chain_, &image_count, nullptr);
  images_.resize(image_count);
  vkGetSwapchainImagesKHR(driver->GetDevice(), chain_, &image_count, &images_[0]);
  format_ = surface_format.format;
  extent_ = extent;
  views_.resize(images_.size());
  for (auto idx = 0; idx < images_.size(); idx++) {
    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = images_[idx];
    create_info.format = format_;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;
    CHECK_VK(FATAL, vkCreateImageView(driver->GetDevice(), &create_info, nullptr, &views_[idx]),
             "failed to create vk image view");
  }

  fence_.Init(driver);
  Publish<SwapChainInitEvent>(reinit);
}

void Renderer::DestroySwapChain(Driver* driver, const bool reinit) {
  ASSERT(driver);
  for (const auto& view : views_) {
    vkDestroyImageView(driver->GetDevice(), view, driver->GetAllocator());
  }
  vkDestroySwapchainKHR(driver->GetDevice(), chain_, driver->GetAllocator());
  Publish<SwapChainDestroyedEvent>(reinit);
}

void Renderer::ReInitSwapChain(Driver* driver) {
  ASSERT(driver);
  const auto window = GetAppWindow();
  ASSERT(window);
  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(window->GetHandle(), &width, &height);
  while (width == 0 && height == 0) {
    glfwGetFramebufferSize(window->GetHandle(), &width, &height);
    glfwWaitEvents();
  }

  driver->WaitDeviceIdle();
  DestroySwapChain(driver, true);
  InitSwapChain(driver, true);
}

auto Renderer::AcquireNextImage(Driver* driver, uint32_t* result) -> bool {
  const auto status = vkAcquireNextImageKHR(driver->GetDevice(), GetSwapChain(), UINT64_MAX,
                                            fence_.AvailableAt(GetCurrentFrame()), VK_NULL_HANDLE, result);
  if (status == VK_ERROR_OUT_OF_DATE_KHR) {
    Renderer::ReInitSwapChain(driver);
    return false;
  } else if (status != VK_SUCCESS && status != VK_SUBOPTIMAL_KHR) {
    LOG(FATAL) << "failed to acquire swap chain image:" << string_VkResult(status);
  }
  return true;
}

void Renderer::Submit(Driver* driver, const VkSemaphore& signal, const std::vector<VkCommandBuffer>& cmd_buffers) {
  ASSERT(driver);
  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  std::array<VkSemaphore, 1> wait_semaphores = {fence_.AvailableAt(GetCurrentFrame())};
  std::array<VkPipelineStageFlags, 1> wait_stages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit_info.waitSemaphoreCount = wait_semaphores.size();
  submit_info.pWaitSemaphores = wait_semaphores.data();
  submit_info.pWaitDstStageMask = wait_stages.data();
  submit_info.commandBufferCount = cmd_buffers.size();
  submit_info.pCommandBuffers = cmd_buffers.data();
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &signal;
  CHECK_VK(FATAL, vkQueueSubmit(driver->GetGraphicsQueue(), 1, &submit_info, fence_.FenceAt(GetCurrentFrame())),
           "failed to submit to vk graphics queue");
}

void Renderer::Present(Driver* driver, const VkSemaphore& signal, const uint32_t image_index) {
  ASSERT(driver);
  VkPresentInfoKHR present_info{};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = &signal;
  present_info.pSwapchains = &Renderer::GetSwapChain();
  present_info.swapchainCount = 1;
  present_info.pImageIndices = &image_index;
  const auto result = vkQueuePresentKHR(driver->GetPresentQueue(), &present_info);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || resized_) {
    resized_ = false;
    Renderer::ReInitSwapChain(driver);
  }
}

void Renderer::DrawFrame(Driver* driver, const Tick& current, const Tick& previous) {
  ASSERT(driver);
  // pre frame
  uint32_t image_index = 0;
  vkWaitForFences(driver->GetDevice(), 1, &fence_.FenceAt(GetCurrentFrame()), VK_TRUE, UINT64_MAX);
  AcquireNextImage(driver, &image_index);
  vkResetFences(driver->GetDevice(), 1, &fence_.FenceAt(GetCurrentFrame()));
  Publish<PreFrameEvent>();

  // draw frame
  std::vector<VkCommandBuffer> cmd_buffers{};
  SceneRenderer::Draw(GetCurrentFrame(), image_index, cmd_buffers);
  GuiRenderer::Draw(GetCurrentFrame(), image_index, cmd_buffers);

  // post frame
  Submit(driver, fence_.FinishedAt(GetCurrentFrame()), cmd_buffers);
  Present(driver, fence_.FinishedAt(GetCurrentFrame()), image_index);
  current_frame_ = ((GetCurrentFrame() + 1) % MAX_NUMBER_OF_FRAMES_IN_FLIGHT);
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
}

void Renderer::Init() {
  OnDriverInitEvent().subscribe([](DriverInitEvent* event) {
    const auto driver = event->GetDriver();
    ASSERT(driver);
    // init swapchain
    InitSwapChain(driver, false);
    // renderer
    InitCommandPool(driver);
    InitResizeListener();
    Publish<RendererInitEvent>();
  });
  engine::OnTerminatingEvent().subscribe([](engine::TerminatingEvent* event) {
    Renderer::Destroy();
  });
}

void Renderer::Destroy() {
  DLOG(INFO) << "destroying vk renderer....";
  const auto driver = Driver::Get();
  ASSERT(driver);
  DestroySwapChain(driver, false);
  fence_.Destroy(driver);
  vkDestroyCommandPool(driver->GetDevice(), command_pool_, driver->GetAllocator());
  Publish<RendererDestroyedEvent>();
}
}  // namespace prt