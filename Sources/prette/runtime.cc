#include "prette/runtime.h"

#include <vector>

#include "prette/class.h"
#include "prette/engine/engine.h"
#include "prette/gfx.h"
#include "prette/mouse/mouse.h"

#include "prette/keyboard/keyboard.h"
#include "prette/os_thread.h"
#include "prette/window/window.h"

#include "prette/runtime_info_printer.h"
#include "prette/signals.h"

namespace prt {
// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
static const auto kMaxFramesInFlight = 2;
static VkInstance vk_instance_{};
static rx::subscription on_terminating_{};
static rx::subscription on_tick_{};
static VkPhysicalDevice vk_physical_device_ = VK_NULL_HANDLE;
static VkDevice vk_device_ = VK_NULL_HANDLE;
static VkSurfaceKHR vk_surface_ = VK_NULL_HANDLE;
static VkQueue vk_queue_ = VK_NULL_HANDLE;
static VkQueue vk_present_queue_ = VK_NULL_HANDLE;
static VkSwapchainKHR vk_swapchain_ = VK_NULL_HANDLE;
static std::vector<VkImage> vk_swapchain_images_;
static VkFormat vk_swapchain_format_;
static VkExtent2D vk_swapchain_extent_;
static std::vector<VkImageView> vk_swapchain_views_;
static std::vector<VkFramebuffer> vk_swapchain_framebuffers_;
static VkPipeline vk_pipeline_;
static VkPipelineLayout vk_pipeline_layout_;
static VkRenderPass vk_render_pass_;
static VkCommandPool vk_cmd_pool_;
static std::array<VkCommandBuffer, kMaxFramesInFlight> vk_cmd_buffers_;
static std::array<VkSemaphore, kMaxFramesInFlight> img_avail_semaphores_;
static std::array<VkSemaphore, kMaxFramesInFlight> render_finished_semaphores_;
static std::array<VkFence, kMaxFramesInFlight> inflight_fences_;
static uint32_t current_frame_ = 0;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)
static const std::vector<const char *> kDeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};
static const std::vector<const char *> kValidationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> modes;
};

static inline auto operator<<(std::ostream &stream,
                              const VkPresentModeKHR &rhs) -> std::ostream & {
  switch (rhs) {
  case VK_PRESENT_MODE_IMMEDIATE_KHR:
    return stream << "Immediate";
  case VK_PRESENT_MODE_MAILBOX_KHR:
    return stream << "Mailbox";
  case VK_PRESENT_MODE_FIFO_KHR:
    return stream << "FIFO";
  case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
    return stream << "FIFO (Relaxed)";
  case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
    return stream << "Demand Refresh";
  case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
    return stream << "Shared Continuous Refresh";
  default:
    return stream << "Unknown VkPresentModeKHR: " << rhs;
  }
}

static inline auto operator<<(std::ostream &stream,
                              const VkSurfaceFormatKHR &rhs) -> std::ostream & {
  return stream << "VkSurfaceFormat("
                << "format=" << rhs.format << ", "
                << "color_space=" << rhs.colorSpace << ")";
}

struct Vertex {
  glm::vec2 pos;
  glm::vec3 color;

  static auto GetBindingDescription() -> VkVertexInputBindingDescription {
    VkVertexInputBindingDescription binding;
    binding.binding = 0;
    binding.stride = sizeof(Vertex);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return binding;
  }

  static auto GetAttributeDescription()
      -> std::array<VkVertexInputAttributeDescription, 2> {
    std::array<VkVertexInputAttributeDescription, 2> attributes{};
    attributes[0].binding = 0;
    attributes[0].location = 0;
    attributes[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributes[0].offset = offsetof(Vertex, pos);

    attributes[1].binding = 0;
    attributes[1].location = 1;
    attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributes[1].offset = offsetof(Vertex, color);
    return attributes;
  }
};

static const std::vector<Vertex> kVertices = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphics;
  std::optional<uint32_t> present;

  auto IsComplete() const -> bool {
    return graphics.has_value() && present.has_value();
  }
};

#ifdef PRT_DEBUG
void PrintRuntimeInfo(const google::LogSeverity s, const char *file,
                      const int line, const int indent) {
  RuntimeInfoPrinter printer(s, file, line, indent);
  return printer.Print();
}
#endif // PRT_DEBUG

void Runtime::OnUnhandledException() {
  CrashReportCause cause(std::current_exception());
  CrashReport report(cause);
  report.Print();
  LOG(FATAL) << "unhandled exception occured.";
}

static inline void InitSyncObjects() {
  VkSemaphoreCreateInfo semaphore{};
  semaphore.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fence{};
  fence.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  for (auto idx = 0; idx < kMaxFramesInFlight; idx++) {
    {
      const auto result = vkCreateSemaphore(vk_device_, &semaphore, nullptr,
                                            &img_avail_semaphores_.at(idx));
      LOG_IF(FATAL, result != VK_SUCCESS)
          << "failed to create img_avail semaphore: " << result;
    }

    {
      const auto result =
          vkCreateSemaphore(vk_device_, &semaphore, nullptr,
                            &render_finished_semaphores_.at(idx));
      LOG_IF(FATAL, result != VK_SUCCESS)
          << "failed to create render_finished semaphore: " << result;
    }

    {
      const auto result =
          vkCreateFence(vk_device_, &fence, nullptr, &inflight_fences_.at(idx));
      LOG_IF(FATAL, result != VK_SUCCESS)
          << "failed to create inflight fence: " << result;
    }
  }
}

static inline auto GetPhysicalDevices() -> rx::observable<VkPhysicalDevice> {
  uint32_t count = 0;
  vkEnumeratePhysicalDevices(vk_instance_, &count, nullptr);
  if (count == 0) {
    LOG(WARNING) << "cannot find any GPUs";
    return rx::observable<>::empty<VkPhysicalDevice>();
  }
  std::vector<VkPhysicalDevice> devices(count);
  vkEnumeratePhysicalDevices(vk_instance_, &count, &devices[0]);
  return rx::observable<>::iterate(devices);
}

static inline void
GetRequiredExtensions(std::vector<const char *> &extensions) {
  uint32_t ext_count = 0;
  const auto required = glfwGetRequiredInstanceExtensions(&ext_count);
  for (auto idx = 0; idx < ext_count; idx++)
    extensions.push_back(required[idx]);
#ifdef OS_IS_OSX
  extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif // OS_IS_OSX
}

static inline auto
operator<<(std::ostream &stream,
           const VkPhysicalDeviceType &rhs) -> std::ostream & {
  switch (rhs) {
  case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_CPU:
    return stream << "CPU";
  case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
    return stream << "Discrete GPU";
  case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
    return stream << "Integrated GPU";
  case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
    return stream << "Virtual GPU";
  case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_OTHER:
  default:
    return stream << "Other";
  }
}

auto FindQueueFamilies(VkPhysicalDevice device) -> QueueFamilyIndices {
  QueueFamilyIndices indices;

  uint32_t count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
  std::vector<VkQueueFamilyProperties> families(count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, &families[0]);

  int i = 0;
  for (const auto &family : families) {
    if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
      indices.graphics = i;
    VkBool32 present = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vk_surface_, &present);
    if (present)
      indices.present = i;
    if (indices.IsComplete())
      break;
    i++;
  }

  return indices;
}

static inline auto
QuerySwapChainSupport(VkPhysicalDevice device,
                      VkSurfaceKHR surface) -> SwapChainSupportDetails {
  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                            &details.capabilities);

  {
    uint32_t count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, nullptr);
    if (count > 0) {
      DLOG(INFO) << "retrieving " << count << " formats for device....";
      details.formats.resize(count);
      vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count,
                                           &details.formats[0]);
    }
    DLOG(INFO) << "found " << count << " formats for device:";
    for (auto idx = 0; idx < count; idx++)
      DLOG(INFO) << "- " << details.formats[idx];
  }

  {
    uint32_t count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, nullptr);
    if (count > 0) {
      DLOG(INFO) << "retrieving " << count << " modes for device....";
      details.modes.resize(count);
      vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count,
                                                &details.modes[0]);
    }
    DLOG(INFO) << "found " << count << " modes for device:";
    for (const auto &mode : details.modes) {
      DLOG(INFO) << "- " << mode;
    }
  }

  return details;
}

static inline auto IsSuitableDeviceType(VkPhysicalDevice device) -> bool {
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(device, &properties);
  return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
         properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
}

static inline auto AreExtensionsSupported(VkPhysicalDevice device) -> bool {
  uint32_t count = 0;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);
  if (count <= 0)
    return false;
  std::vector<VkExtensionProperties> available(count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &count, &available[0]);
  std::set<std::string> required(std::begin(kDeviceExtensions),
                                 std::end(kDeviceExtensions));
  for (const auto &ext : available)
    required.erase(ext.extensionName);
  return required.empty();
}

static inline auto IsSwapChainOk(VkPhysicalDevice device) -> bool {
  const auto support = QuerySwapChainSupport(device, vk_surface_);
  return !support.formats.empty() && !support.modes.empty();
}

static inline auto IsSuitableDevice(VkPhysicalDevice device) -> bool {
  QueueFamilyIndices indices = FindQueueFamilies(device);
  const auto extensions_supported = AreExtensionsSupported(device);
  DLOG(INFO) << "extensions supported: " << (extensions_supported ? 'y' : 'n');
  const auto swapchain_ok = IsSwapChainOk(device);
  DLOG(INFO) << "swap chain ok: " << (swapchain_ok ? 'y' : 'n');
  return IsSuitableDeviceType(device) && indices.IsComplete() &&
         AreExtensionsSupported(device) && IsSwapChainOk(device);
}

static inline void InitInstance() {
  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Hello World";
  app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
  app_info.pEngineName = "No Engine";
  app_info.engineVersion = VK_MAKE_VERSION(0, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_3;

  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;
  create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  create_info.enabledLayerCount = 0;
  std::vector<const char *> extensions;
  GetRequiredExtensions(extensions);
  create_info.enabledExtensionCount = extensions.size();
  create_info.ppEnabledExtensionNames = &extensions[0];

#ifdef PRT_DEBUG
  DLOG(INFO) << "extensions:";
  for (const auto &ext : extensions)
    DLOG(INFO) << "- " << ext;
#endif // PRT_DEBUG

  {
    const auto result = vkCreateInstance(&create_info, nullptr, &vk_instance_);
    LOG_IF(FATAL, result != VK_SUCCESS)
        << "failed to create vkInstance: " << result;
  }
}

static inline void InitSurface() {
  const auto window = window::GetAppWindow();
  PRT_ASSERT(window);
  const auto result = glfwCreateWindowSurface(vk_instance_, window->GetHandle(),
                                              nullptr, &vk_surface_);
  LOG_IF(FATAL, result != VK_SUCCESS)
      << "failed to create vkService: " << result;
}

static inline void PrintPhysicalDevice(const VkPhysicalDevice &device) {
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(device, &properties);
  DLOG(INFO) << " * ID: " << properties.deviceID;
  DLOG(INFO) << " * Name: " << properties.deviceName;
  DLOG(INFO) << " * Type: " << properties.deviceType;
  DLOG(INFO) << " * Vendor: " << properties.vendorID;
}

static inline void PickPhysicalDevice() {
  DLOG(INFO) << "choosing physical device....";
  vk_physical_device_ =
      GetPhysicalDevices().filter(IsSuitableDevice).as_blocking().first();
  LOG_IF(FATAL, vk_physical_device_ == VK_NULL_HANDLE)
      << "failed to find a suitable physical device.";
  DLOG(INFO) << "chose: ";
  PrintPhysicalDevice(vk_physical_device_);
}

static inline void InitLogicalDevice() {
  const auto indices = FindQueueFamilies(vk_physical_device_);
  const auto priority = 1.0f;
  std::vector<VkDeviceQueueCreateInfo> create_infos{};
  std::set<uint32_t> queue_indices = {
      indices.graphics.value(),
      indices.present.value(),
  };
  for (const auto index : queue_indices) {
    VkDeviceQueueCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    create_info.queueFamilyIndex = index;
    create_info.queueCount = 1;
    create_info.pQueuePriorities = &priority;
    create_infos.push_back(create_info);
  }

  VkPhysicalDeviceFeatures features{};

  VkDeviceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  // extensions
  create_info.enabledExtensionCount = kDeviceExtensions.size();
  create_info.ppEnabledExtensionNames = &kDeviceExtensions[0];
  // layers
  create_info.enabledLayerCount = 0;
  // create_infos
  create_info.pQueueCreateInfos = &create_infos[0];
  create_info.queueCreateInfoCount = create_infos.size();
  // features
  create_info.pEnabledFeatures = &features;

  {
    const auto result =
        vkCreateDevice(vk_physical_device_, &create_info, nullptr, &vk_device_);
    LOG_IF(FATAL, result != VK_SUCCESS)
        << "failed to create vkDevice: " << result;
  }

  vkGetDeviceQueue(vk_device_, indices.graphics.value(), 0, &vk_queue_);
  vkGetDeviceQueue(vk_device_, indices.present.value(), 0, &vk_present_queue_);
}

static inline auto
ChooseSwapMode(const std::vector<VkPresentModeKHR> &modes) -> VkPresentModeKHR {
  for (const auto &mode : modes) {
    if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
      return mode;
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

static inline auto ChooseSwapFormat(
    const std::vector<VkSurfaceFormatKHR> &formats) -> VkSurfaceFormatKHR {
  for (const auto &format : formats) {
    if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
        format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      return format;
  }
  return formats[0];
}

static inline auto
ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) -> VkExtent2D {
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    return capabilities.currentExtent;
  const auto window = GetAppWindow();
  PRT_ASSERT(window);
  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(window->GetHandle(), &width, &height);

  VkExtent2D extent = {static_cast<uint32_t>(width),
                       static_cast<uint32_t>(height)};
  extent.width = std::clamp(extent.width, capabilities.minImageExtent.width,
                            capabilities.maxImageExtent.width);
  extent.height = std::clamp(extent.height, capabilities.maxImageExtent.height,
                             capabilities.maxImageExtent.height);

  return extent;
}

static inline void InitSwapChain() {
  DLOG(INFO) << "creating SwapChain....";
  const auto support = QuerySwapChainSupport(vk_physical_device_, vk_surface_);
  const auto format = ChooseSwapFormat(support.formats);
  const auto mode = ChooseSwapMode(support.modes);
  const auto extent = ChooseSwapExtent(support.capabilities);

  uint32_t img_count = support.capabilities.minImageCount + 1;
  if (support.capabilities.maxImageCount > 0 &&
      img_count > support.capabilities.maxImageCount)
    img_count = support.capabilities.maxImageCount;

  VkSwapchainCreateInfoKHR create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = vk_surface_;
  create_info.minImageCount = img_count;
  create_info.imageFormat = format.format;
  create_info.imageColorSpace = format.colorSpace;
  create_info.imageExtent = extent;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  const auto families = FindQueueFamilies(vk_physical_device_);
  uint32_t indices[] = {
      families.graphics.value(),
      families.present.value(),
  };

  if (families.graphics != families.present) {
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = indices;
  } else {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  create_info.preTransform = support.capabilities.currentTransform;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  create_info.presentMode = mode;
  create_info.clipped = VK_TRUE;
  create_info.oldSwapchain = VK_NULL_HANDLE;

  const auto result =
      vkCreateSwapchainKHR(vk_device_, &create_info, nullptr, &vk_swapchain_);
  LOG_IF(FATAL, result != VK_SUCCESS)
      << "failed to create vkSwapChain: " << result;

  uint32_t count = 0;
  vkGetSwapchainImagesKHR(vk_device_, vk_swapchain_, &count, nullptr);
  if (count <= 0)
    return;
  vk_swapchain_images_.resize(count);
  vkGetSwapchainImagesKHR(vk_device_, vk_swapchain_, &count,
                          &vk_swapchain_images_[0]);
  vk_swapchain_format_ = format.format;
  vk_swapchain_extent_ = extent;
}

static inline void InitImageViews() {
  const auto total_views = vk_swapchain_images_.size();
  vk_swapchain_views_.resize(total_views);
  for (auto idx = 0; idx < total_views; idx++) {
    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = vk_swapchain_images_[idx];
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = vk_swapchain_format_;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    const auto result = vkCreateImageView(vk_device_, &create_info, nullptr,
                                          &vk_swapchain_views_[idx]);
    LOG_IF(FATAL, result != VK_SUCCESS)
        << "failed to create vkImageView: " << result;
  }
}

static inline void InitCommandPool() {
  const auto indices = FindQueueFamilies(vk_physical_device_);
  VkCommandPoolCreateInfo pool{};
  pool.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  pool.queueFamilyIndex = indices.graphics.value();
  const auto result =
      vkCreateCommandPool(vk_device_, &pool, nullptr, &vk_cmd_pool_);
  LOG_IF(FATAL, result != VK_SUCCESS)
      << "failed to create vkCommandPool: " << result;
}

static inline void InitCommandBuffer() {
  VkCommandBufferAllocateInfo alloc;
  alloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc.commandPool = vk_cmd_pool_;
  alloc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc.commandBufferCount = kMaxFramesInFlight;
  const auto result =
      vkAllocateCommandBuffers(vk_device_, &alloc, &vk_cmd_buffers_[0]);
  LOG_IF(FATAL, result != VK_SUCCESS)
      << "failed to allocate vkCommandBuffer: " << result;
}

static inline void InitFramebuffers() {
  const auto total_framebuffers = vk_swapchain_views_.size();
  vk_swapchain_framebuffers_.resize(total_framebuffers);
  for (auto i = 0; i < total_framebuffers; i++) {
    VkImageView attachments[] = {vk_swapchain_views_[i]};
    VkFramebufferCreateInfo framebuffer{};
    framebuffer.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer.renderPass = vk_render_pass_;
    framebuffer.attachmentCount = 1;
    framebuffer.pAttachments = attachments;
    framebuffer.width = vk_swapchain_extent_.width;
    framebuffer.height = vk_swapchain_extent_.height;
    framebuffer.layers = 1;

    const auto result = vkCreateFramebuffer(vk_device_, &framebuffer, nullptr,
                                            &vk_swapchain_framebuffers_[i]);
    LOG_IF(FATAL, result != VK_SUCCESS)
        << "failed to create vkFramebuffer: " << result;
  }
}

static inline void RecordCommandBuffer(VkCommandBuffer buf, uint32_t idx) {
  VkCommandBufferBeginInfo begin{};
  begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  {
    const auto result = vkBeginCommandBuffer(buf, &begin);
    LOG_IF(FATAL, result != VK_SUCCESS)
        << "failed to begin vkCommandBuffer: " << result;
  }

  VkRenderPassBeginInfo render_pass{};
  render_pass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass.renderPass = vk_render_pass_;
  render_pass.framebuffer = vk_swapchain_framebuffers_[idx];
  render_pass.renderArea.offset = {0, 0};
  render_pass.renderArea.extent = vk_swapchain_extent_;

  VkClearValue clear = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  render_pass.clearValueCount = 1;
  render_pass.pClearValues = &clear;

  vkCmdBeginRenderPass(buf, &render_pass, VK_SUBPASS_CONTENTS_INLINE);
  {
    vkCmdBindPipeline(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline_);
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(vk_swapchain_extent_.width);
    viewport.height = static_cast<float>(vk_swapchain_extent_.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(buf, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = vk_swapchain_extent_;
    vkCmdSetScissor(buf, 0, 1, &scissor);

    vkCmdDraw(buf, 3, 1, 0, 0);
  }
  vkCmdEndRenderPass(buf);

  {
    const auto result = vkEndCommandBuffer(buf);
    LOG_IF(FATAL, result != VK_SUCCESS)
        << "failed to end vkCommandBuffer: " << result;
  }
}

static inline void DestroyInstance() {
  for (auto i = 0; i < kMaxFramesInFlight; i++) {
    vkDestroySemaphore(vk_device_, render_finished_semaphores_[i], nullptr);
    vkDestroySemaphore(vk_device_, img_avail_semaphores_[i], nullptr);
    vkDestroyFence(vk_device_, inflight_fences_[i], nullptr);
  }
  vkDestroyCommandPool(vk_device_, vk_cmd_pool_, nullptr);
  for (const auto &framebuffer : vk_swapchain_framebuffers_)
    vkDestroyFramebuffer(vk_device_, framebuffer, nullptr);
  vkDestroyPipeline(vk_device_, vk_pipeline_, nullptr);
  vkDestroyPipelineLayout(vk_device_, vk_pipeline_layout_, nullptr);
  vkDestroyRenderPass(vk_device_, vk_render_pass_, nullptr);
  for (const auto &view : vk_swapchain_views_)
    vkDestroyImageView(vk_device_, view, nullptr);
  vkDestroySwapchainKHR(vk_device_, vk_swapchain_, nullptr);
  vkDestroyDevice(vk_device_, nullptr);
  vkDestroySurfaceKHR(vk_instance_, vk_surface_, nullptr);
  vkDestroyInstance(vk_instance_, nullptr);
}

static inline auto
GetShaderCode(const std::string &filename) -> std::vector<char> {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);
  if (!file.is_open())
    throw std::runtime_error(
        fmt::format("failed to open file: {0:s}", filename));
  const auto fsize = file.tellg();
  std::vector<char> buffer(fsize);
  file.seekg(0);
  file.read(&buffer[0], fsize);
  file.close();
  return buffer;
}

static inline void CreateShaderModule(VkShaderModule &shader_module,
                                      const std::vector<char> &code) {
  VkShaderModuleCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size();
  create_info.pCode = (const uint32_t *)&code[0];
  const auto result =
      vkCreateShaderModule(vk_device_, &create_info, nullptr, &shader_module);
  LOG_IF(FATAL, result != VK_SUCCESS)
      << "failed to create vkShaderModule: " << result;
}

static inline void
InitVertexShaderInfo(VkPipelineShaderStageCreateInfo &create_info,
                     VkShaderModule &shader_module) {
  const auto filename = fmt::format("{0:s}/shaders/vert.spv", FLAGS_resources);
  DLOG(INFO) << "loading vertex shader from: " << filename;
  const auto code = GetShaderCode(filename);
  PRT_ASSERT(!code.empty());
  CreateShaderModule(shader_module, code);
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  create_info.module = shader_module;
  create_info.pName = "main";
}

static inline void
InitFragmentShaderInfo(VkPipelineShaderStageCreateInfo &create_info,
                       VkShaderModule &shader_module) {
  const auto filename = fmt::format("{0:s}/shaders/frag.spv", FLAGS_resources);
  DLOG(INFO) << "loading vertex shader from: " << filename;
  const auto code = GetShaderCode(filename);
  PRT_ASSERT(!code.empty());
  CreateShaderModule(shader_module, code);
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  create_info.module = shader_module;
  create_info.pName = "main";
}

static inline void InitRenderPass() {
  VkAttachmentDescription color_attachment{};
  color_attachment.format = vk_swapchain_format_;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_ref{};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref;

  VkRenderPassCreateInfo render_pass{};
  render_pass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass.attachmentCount = 1;
  render_pass.pAttachments = &color_attachment;
  render_pass.subpassCount = 1;
  render_pass.pSubpasses = &subpass;

  const auto result =
      vkCreateRenderPass(vk_device_, &render_pass, nullptr, &vk_render_pass_);
  LOG_IF(FATAL, result != VK_SUCCESS)
      << "failed to create render pass: " << result;
}

static inline void InitGraphicsPipeline() {
  VkShaderModule vert_module{};
  VkPipelineShaderStageCreateInfo vert_create_info{};
  InitVertexShaderInfo(vert_create_info, vert_module);

  VkShaderModule frag_module{};
  VkPipelineShaderStageCreateInfo frag_create_info{};
  InitFragmentShaderInfo(frag_create_info, frag_module);

  VkPipelineShaderStageCreateInfo stages[] = {
      vert_create_info,
      frag_create_info,
  };

  VkPipelineVertexInputStateCreateInfo vertex_input_info{};
  vertex_input_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount = 0;
  vertex_input_info.vertexAttributeDescriptionCount = 0;

  VkPipelineInputAssemblyStateCreateInfo input_assembly{};
  input_assembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  VkPipelineViewportStateCreateInfo view_state{};
  view_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  view_state.viewportCount = 1;
  view_state.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo rasterizer;
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendAttachmentState color_blend_attachment{};
  color_blend_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo color_blend{};
  color_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend.logicOpEnable = VK_FALSE;
  color_blend.logicOp = VK_LOGIC_OP_COPY;
  color_blend.attachmentCount = 1;
  color_blend.pAttachments = &color_blend_attachment;
  color_blend.blendConstants[0] = 0.0f;
  color_blend.blendConstants[1] = 0.0f;
  color_blend.blendConstants[2] = 0.0f;
  color_blend.blendConstants[3] = 0.0f;

  std::vector<VkDynamicState> dynamic_states = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR,
  };
  VkPipelineDynamicStateCreateInfo dynamic_state{};
  dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state.dynamicStateCount = dynamic_states.size();
  dynamic_state.pDynamicStates = &dynamic_states[0];

  VkPipelineLayoutCreateInfo pipeline_layout{};
  pipeline_layout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout.setLayoutCount = 0;
  pipeline_layout.pushConstantRangeCount = 0;

  {
    const auto result = vkCreatePipelineLayout(vk_device_, &pipeline_layout,
                                               nullptr, &vk_pipeline_layout_);
    LOG_IF(FATAL, result != VK_SUCCESS)
        << "failed to create vkPipelineLayout: " << result;
  }

  VkGraphicsPipelineCreateInfo pipeline{};
  pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline.stageCount = 2;
  pipeline.pStages = stages;
  pipeline.pVertexInputState = &vertex_input_info;
  pipeline.pInputAssemblyState = &input_assembly;
  pipeline.pViewportState = &view_state;
  pipeline.pRasterizationState = &rasterizer;
  pipeline.pMultisampleState = &multisampling;
  pipeline.pColorBlendState = &color_blend;
  pipeline.pDynamicState = &dynamic_state;
  pipeline.layout = vk_pipeline_layout_;
  pipeline.renderPass = vk_render_pass_;
  pipeline.subpass = 0;
  pipeline.basePipelineHandle = VK_NULL_HANDLE;

  {
    const auto result = vkCreateGraphicsPipelines(
        vk_device_, VK_NULL_HANDLE, 1, &pipeline, nullptr, &vk_pipeline_);
    LOG_IF(FATAL, result != VK_SUCCESS)
        << "failed to create vkGraphicsPipeline: " << result;
  }

  vkDestroyShaderModule(vk_device_, frag_module, nullptr);
  vkDestroyShaderModule(vk_device_, vert_module, nullptr);
}

static inline void DrawFrame() {
  const auto &buffer = vk_cmd_buffers_.at(current_frame_);
  const auto &fence = inflight_fences_.at(current_frame_);
  const auto &finished_semaphore =
      render_finished_semaphores_.at(current_frame_);
  const auto &avail_semaphore = img_avail_semaphores_.at(current_frame_);
  vkWaitForFences(vk_device_, 1, &fence, VK_TRUE, UINT64_MAX);
  vkResetFences(vk_device_, 1, &fence);

  uint32_t index = 0;
  vkAcquireNextImageKHR(vk_device_, vk_swapchain_, UINT64_MAX,
                        img_avail_semaphores_.at(current_frame_),
                        VK_NULL_HANDLE, &index);

  vkResetCommandBuffer(buffer, 0);
  RecordCommandBuffer(buffer, index);

  VkSubmitInfo submit{};
  submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore wait_semaphores[] = {avail_semaphore};
  VkPipelineStageFlags wait_stages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit.waitSemaphoreCount = 1;
  submit.pWaitSemaphores = wait_semaphores;
  submit.pWaitDstStageMask = wait_stages;
  submit.commandBufferCount = 1;
  submit.pCommandBuffers = &buffer;

  VkSemaphore signals[] = {finished_semaphore};
  submit.signalSemaphoreCount = 1;
  submit.pSignalSemaphores = signals;

  {
    const auto result = vkQueueSubmit(vk_queue_, 1, &submit, fence);
    LOG_IF(FATAL, result != VK_SUCCESS)
        << "failed to submit vkQueue: " << result;
  }

  VkPresentInfoKHR present_info{};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signals;

  VkSwapchainKHR swapchains[] = {
      vk_swapchain_,
  };
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swapchains;
  present_info.pImageIndices = &index;

  vkQueuePresentKHR(vk_present_queue_, &present_info);
  current_frame_ = (current_frame_ + 1) % kMaxFramesInFlight;
}

void Runtime::Init(int argc, char **argv) {
  srand(time(nullptr));

  // ::google::InstallPrefixFormatter(&MyPrefixFormatter);
  ::google::InitGoogleLogging(argv[0]);
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  InitSignalHandlers();
  std::set_terminate(OnUnhandledException);
  LOG_IF(FATAL, !SetCurrentThreadName("main"))
      << "failed to set main thread name.";
  gfx::Init();
  engine::InitEngine();
  window::InitWindows();
  mouse::InitMouse();
  keyboard::InitKeyboard();
  Class::Init();

  InitInstance();
  InitSurface();
  PickPhysicalDevice();
  InitLogicalDevice();
  InitSwapChain();
  InitImageViews();
  InitRenderPass();
  InitGraphicsPipeline();
  InitFramebuffers();
  InitCommandPool();
  InitCommandBuffer();
  InitSyncObjects();

  const auto engine = GetEngine();
  PRT_ASSERT(engine);
  on_terminating_ = engine
                        ->OnTerminating() // NOLINT(cppcoreguidelines-slicing)
                        .subscribe([](engine::TerminatingEvent *event) {
                          vkDeviceWaitIdle(vk_device_);
                          DestroyInstance();
                        });
  on_tick_ = engine
                 ->OnTick() // NOLINT(cppcoreguidelines-slicing)
                 .subscribe([](engine::TickEvent *event) { DrawFrame(); });
}

auto Runtime::Run() -> int {
#ifdef PRT_DEBUG
  PrintRuntimeInfo();
#endif // PRT_DEBUG
  const auto engine = GetEngine();
  PRT_ASSERT(engine);
  engine->Run();
  return EXIT_SUCCESS;
}
} // namespace prt