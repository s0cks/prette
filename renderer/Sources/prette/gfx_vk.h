#ifndef PRT_GFX_VK_H
#define PRT_GFX_VK_H

#include <algorithm>
#include <glog/logging.h>
#include <optional>
#include <ostream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/gfx_driver.h"
#include "prette/glm.h"
#include "prette/prette.h"
#include "prette/vk.h"

#ifndef VK_FLAGS_NONE
#define VK_FLAGS_NONE 0
#endif  // VK_FLAGS_NONE

namespace prt {
class Driver;

static inline auto operator==(const VkExtent2D& lhs, const VkExtent2D& rhs) -> bool {
  return lhs.width == rhs.width && lhs.height == rhs.height;
}

static inline auto operator!=(const VkExtent2D& lhs, const VkExtent2D& rhs) -> bool {
  return lhs.width != rhs.width || lhs.height != rhs.height;
}

static inline auto operator<<(std::ostream& stream, const VkResult& rhs) -> std::ostream& {
  return stream << std::string(string_VkResult(rhs));
}

#define CHECK_VK(Severity, Status, Message)                   \
  ({                                                          \
    vk::Result result = (Status);                             \
    LOG_IF(Severity, !result) << (Message) << ": " << result; \
  })

struct QueueFamilyIndices {
  std::optional<uint32_t> graphics{};
  std::optional<uint32_t> present{};

  auto GetGraphicsFamily() const -> uint32_t {
    ASSERT(graphics.has_value());
    return graphics.value();
  }

  auto GetPresentFamily() const -> uint32_t {
    ASSERT(graphics.has_value());
    return graphics.value();
  }

  auto IsComplete() const -> bool {
    return graphics.has_value() && present.has_value();
  }

  void GetUniqueFamilies(std::unordered_set<uint32_t>& families) const {
    families.reserve(2);
    families.insert(present.value());
    families.insert(graphics.value());
  }

  void GetGraphicsQueue(const VkDevice& device, VkQueue& queue, const int index = 0) const {
    vkGetDeviceQueue(device, GetGraphicsFamily(), index, &queue);
  }

  void GetPresentQueue(const VkDevice& device, VkQueue& queue, const int index = 0) const {
    vkGetDeviceQueue(device, GetPresentFamily(), index, &queue);
  }
};

#define VK_MAKE_PRT_VERSION VK_MAKE_VERSION(PRT_VERSION_MAJOR, PRT_VERSION_MINOR, PRT_VERSION_PATCH)

static inline auto ClampExtent(VkExtent2D& v, const VkExtent2D& min, const VkExtent2D& max) -> VkExtent2D& {
  v.width = std::clamp(v.width, min.width, max.width);
  v.height = std::clamp(v.height, min.height, max.height);
  return v;
}

static inline auto ClampExtent(VkExtent2D& v, const VkSurfaceCapabilitiesKHR& surface_capabilities) -> VkExtent2D& {
  return ClampExtent(v, surface_capabilities.minImageExtent, surface_capabilities.maxImageExtent);
}

static inline auto FindQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface)
    -> QueueFamilyIndices {
  QueueFamilyIndices indices{};
  uint32_t count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
  std::vector<VkQueueFamilyProperties> families(count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, &families[0]);
  auto idx = 0;
  for (const auto& family : families) {
    if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics = idx;
    }

    VkBool32 present_supported = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, idx, surface, &present_supported);
    if (present_supported) {
      indices.present = idx;
    }

    if (indices.IsComplete()) {
      break;
    }

    idx++;
  }
  return indices;
}

#ifdef PRT_DEBUG

static inline void PrintProperties(const VkPhysicalDeviceProperties properties) {
  LOG(INFO) << "  * Device Name: " << properties.deviceName;
  LOG(INFO) << "  * Device Type: " << properties.deviceType;
  LOG(INFO) << "  * Vendor ID: " << properties.vendorID;
  LOG(INFO) << "  * Device ID: " << properties.deviceID;
  LOG(INFO) << "  * API Version: " << VK_VERSION_MAJOR(properties.apiVersion) << "."
            << VK_VERSION_MINOR(properties.apiVersion) << "." << VK_VERSION_PATCH(properties.apiVersion);
}

static inline void PrintProperties(const VkPhysicalDevice& device) {
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(device, &properties);
  return PrintProperties(std::move(properties));
}

#endif  // PRT_DEBUG

namespace engine {
class InitState;
class TerminatedState;
}  // namespace engine

#define FOR_EACH_VK_HANDLE(V) \
  V(Image)                    \
  V(ImageView)                \
  V(PipelineLayout)           \
  V(Framebuffer)              \
  V(RenderPass)               \
  V(Sampler)                  \
  V(Semaphore)                \
  V(Fence)                    \
  V(DescriptorSetLayout)      \
  V(ShaderModule)             \
  V(PipelineCache)            \
  V(CommandPool)              \
  V(Buffer)

class Window;
class Driver : public DriverBase {
  friend class Runtime;
  friend class DriverBase;
  friend class DriverInitializer;
  friend class InitState;
  friend class TerminatedState;
  friend class GuiRenderPass;
  DEFINE_NON_COPYABLE_TYPE(Driver);

 private:
  VkApplicationInfo app_info_{};
  vk::Instance* instance_{};
  vk::PhysicalDevice* physical_device_;
  vk::Device* device_;
  vk::Surface* surface_;
  vk::CommandPool* command_pool_ = nullptr;
  VkDescriptorPool descriptor_pool_{};
  VkAllocationCallbacks* allocator_ = nullptr;

  Driver();
  void InitApplicationInfo();
  void InitSurface();
  void InitCommandPool();
  void InitDescriptorPool();

 public:
  ~Driver() override;

  auto GetApplicationInfo() const -> const VkApplicationInfo& {
    return app_info_;
  }

  auto GetInstance() const -> vk::Instance* {
    return instance_;
  }

  auto GetPhysicalDevice() const -> vk::PhysicalDevice* {
    return physical_device_;
  }

  auto GetDevice() const -> vk::Device* {
    return device_;
  }

  auto GetAllocator() const -> VkAllocationCallbacks* {
    return allocator_;
  }

  auto GetSurface() const -> vk::Surface* {
    return surface_;
  }

  auto GetCommandPool() const -> vk::CommandPool* {
    return command_pool_;
  }

  auto GetDescriptorPool() const -> VkDescriptorPool const& {
    return descriptor_pool_;
  }

  void WaitDeviceIdle() const;
  auto GetDepthFormat() const -> VkFormat;

  void CreateFence(VkFence& fence, const VkFenceCreateFlags flags = 0x0);
  void CreateSemaphore(VkSemaphore& semaphore, const VkSemaphoreCreateFlags flags = 0x0);
  void CreateRenderPass(const VkRenderPassCreateInfo& create_info, VkRenderPass& pass);

  void CreateGraphicsPipeline(const VkGraphicsPipelineCreateInfo& create_info, const VkPipelineCache& cache,
                              VkPipeline* result);

  void CreateSwapchain(const VkSwapchainCreateInfoKHR* info, VkSwapchainKHR* result);
  void CreateInstance(const VkInstanceCreateInfo* create_info, VkInstance* result);
  void CreateDevice(const VkDeviceCreateInfo* create_info, VkDevice* result);
#ifdef PRT_DEBUG
  void CreateDebugUtilsMessenger(const VkInstance& instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info,
                                 VkDebugUtilsMessengerEXT* result);
#endif  // PRT_DEBUG
  // clang-format off
#define DECLARE_CREATE(Type) \
  void Create##Type(const Vk##Type##CreateInfo* info, Vk##Type* result);
  FOR_EACH_VK_HANDLE(DECLARE_CREATE)
#undef DECLARE_CREATE
  // clang-format on

  void DestroySwapchain(const VkSwapchainKHR& rhs);
  void DestroyInstance(const VkInstance& rhs);
  void DestroyPipeline(const VkPipeline& rhs);
#ifdef PRT_DEBUG
  void DestroyDebugUtilsMessenger(const VkDebugUtilsMessengerEXT& rhs);
#endif  // PRT_DEBUG
  // clang-format off
#define DECLARE_DESTROY(Type) \
  void Destroy##Type(const Vk##Type& result);
  FOR_EACH_VK_HANDLE(DECLARE_DESTROY)
#undef DECLARE_DESTROY
  // clang-format on

  void Destroy(const VkDeviceMemory& memory);
};

static inline auto GetDriverDevice() -> vk::Device* {
  ASSERT(Driver::IsInitialized());
  return Driver::Get()->GetDevice();
}

static inline auto GetDriverPhysicalDevice() -> vk::Device* {
  ASSERT(Driver::IsInitialized());
  return Driver::Get()->GetDevice();
}

static inline auto GetDriverSurface() -> vk::Surface* {
  ASSERT(Driver::IsInitialized());
  return Driver::Get()->GetSurface();
}

namespace vk {
static constexpr const auto kDefaultViewportMinDepth = 0.0f;
static constexpr const auto kDefaultViewportMaxDepth = 1.0f;

static inline void SetViewport(const VkCommandBuffer& buffer, const glm::vec2 pos, const glm::vec2 size,
                               const float minDepth = kDefaultViewportMinDepth,
                               const float maxDepth = kDefaultViewportMaxDepth) {
  VkViewport viewport{
      .x = pos.x,
      .y = pos.y,
      .width = size[0],
      .height = size[1],
      .minDepth = minDepth,
      .maxDepth = maxDepth,
  };
  vkCmdSetViewport(buffer, 0, 1, &viewport);
}

static inline void SetViewport(const VkCommandBuffer& buffer, const glm::vec2 size,
                               const float minDepth = kDefaultViewportMinDepth,
                               const float maxDepth = kDefaultViewportMaxDepth) {
  return SetViewport(buffer, glm::vec2(0.0f), std::move(size), minDepth, maxDepth);
}

static inline void SetScissor(const VkCommandBuffer& buffer, const glm::i32vec2 offset, const glm::u32vec2 extent) {
  const auto scissor = VkRect2D{
      .offset = {.x = offset.x, .y = offset.y},
      .extent = {.width = extent[0], .height = extent[1]},
  };
  vkCmdSetScissor(buffer, 0, 1, &scissor);
}

static inline void SetScissor(const VkCommandBuffer& buffer, const glm::u32vec2 extent) {
  return SetScissor(buffer, glm::i32vec2(0), std::move(extent));
}
}  // namespace vk
}  // namespace prt

#include "prette/vk_cmd_buffers.h"  // IWYU pragma: keep

#endif  // PRT_GFX_VK_H
