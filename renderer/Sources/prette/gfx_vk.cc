#include "prette/gfx_vk.h"

#include <algorithm>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/command_pool.h"
#include "prette/common.h"
#include "prette/device.h"  // IWYU pragma: keep
#include "prette/device_builder.h"
#include "prette/gfx.h"
#include "prette/gfx_driver_event.h"
#include "prette/render_pass/scene_renderer.h"
#include "prette/surface.h"
#include "prette/vk.h"
#include "prette/vk_instance.h"
#include "prette/vk_physical_device.h"

namespace prt {
static inline auto HasValidationLayerSupport(const vk::LayerSet& layers) -> bool {
  uint32_t count = 0;
  vkEnumerateInstanceLayerProperties(&count, nullptr);

  std::vector<VkLayerProperties> available(count);
  vkEnumerateInstanceLayerProperties(&count, &available[0]);

  for (const auto& name : layers) {
    const auto pos = std::ranges::find_if(available, [name](const VkLayerProperties& props) {
      return name.compare(props.layerName) == 0;
    });
    if (pos == std::end(available))
      return false;
  }
  return true;
}

void Driver::InitApplicationInfo() {
  app_info_.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info_.pApplicationName = "Prette";
  app_info_.applicationVersion = VK_MAKE_PRT_VERSION;
  app_info_.pEngineName = "Prette";
  app_info_.engineVersion = VK_MAKE_PRT_VERSION;
  app_info_.pNext = nullptr;
  app_info_.apiVersion = VK_API_VERSION_1_3;
}

void Driver::WaitDeviceIdle() const {
  vkDeviceWaitIdle(*GetDevice());
}

static inline auto HasStencilComponent(const VkFormat format) -> bool {
  return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

auto Driver::GetDepthFormat() const -> VkFormat {
  return GetPhysicalDevice()->GetDepthFormat();
}

static inline void GetRequiredLayers(vk::LayerSet& layers) {
#ifdef PRT_DEBUG
  layers.insert("VK_LAYER_KHRONOS_validation");
#endif  // PRT_DEBUG
}

static inline void GetRequiredExtensions(vk::ExtensionSet& extensions) {
  uint32_t ext_count = 0;
  const auto glfw_exts = glfwGetRequiredInstanceExtensions(&ext_count);
  for (auto idx = 0; idx < ext_count; idx++) {
    extensions.insert(glfw_exts[idx]);
  }
#ifdef OS_IS_OSX
  extensions.insert(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif  // OS_IS_OSX
#ifdef PRT_DEBUG
  extensions.insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif  // PRT_DEBUG
}

void Driver::InitCommandPool() {
  const auto& queue_families = GetPhysicalDevice()->GetQueueFamilies();

  vk::CommandPoolBuilder builder{};
  // clang-format off
  command_pool_ = builder.WithResetCommandBufferFlag()
    .WithQueueFamilyIndex(queue_families.GetGraphicsFamily())
    .Build();
  // clang-format on
}

void Driver::InitDescriptorPool() {
  // clang-format off
  static const std::vector<VkDescriptorPoolSize> kDescriptorPoolSizes = {
    {.type = VK_DESCRIPTOR_TYPE_SAMPLER, .descriptorCount = 1000},
    {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = 1000},
    {.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, .descriptorCount = 1000},
    {.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, .descriptorCount = 1000},
    {.type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, .descriptorCount = 1000},
    {.type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, .descriptorCount = 1000},
    {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 1000},
    {.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = 1000},
    {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, .descriptorCount = 1000},
    {.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, .descriptorCount = 1000},
    {.type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, .descriptorCount = 1000}
  };
  // clang-format on
  VkDescriptorPoolCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  create_info.flags |= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  create_info.poolSizeCount = kDescriptorPoolSizes.size();
  create_info.pPoolSizes = kDescriptorPoolSizes.data();
  create_info.maxSets = 1000;
  CHECK_VK(FATAL, vkCreateDescriptorPool(*GetDevice(), &create_info, GetAllocator(), &descriptor_pool_),
           "failed to create vk descriptor pool");
}

static vk::LayerSet kInstanceLayers = {
#ifdef PRT_DEBUG
    "VK_LAYER_KHRONOS_validation",
#endif  // PRT_DEBUG
};

Driver::Driver() {
#ifdef PRT_DEBUG
  LOG_IF(FATAL, !HasValidationLayerSupport(kInstanceLayers)) << "vk validation layers requested but not available!";
#endif  // PRT_DEBUG
  InitApplicationInfo();
  {
    // instance
    vk::ExtensionSet required_extensions{};
    GetRequiredExtensions(required_extensions);

    vk::LayerSet required_layers{};
    GetRequiredLayers(required_layers);
    LOG_IF(FATAL, !HasValidationLayerSupport(required_layers)) << "";

    vk::InstanceBuilder builder(&GetApplicationInfo());
    // clang-format off
    instance_ = builder.WithLayers(required_layers)
      .WithExtensions(required_extensions)
      .Build();
    // clang-format on
    ASSERT_INITIALIZED(instance_);
    Publish<InstanceInitEvent>();
  }
  {
    surface_ = new vk::Surface(this);
    ASSERT_INITIALIZED(surface_);
    Publish<SurfaceInitEvent>();
  }
  {
    // physical device
    vk::PhysicalDeviceResolver resolver{};
    physical_device_ = resolver.Resolve(*instance_, *surface_);
    ASSERT_INITIALIZED(physical_device_);
    Publish<PhysicalDeviceInitEvent>();
  }
  {
    // device
    // clang-format off
    vk::DeviceBuilder builder{};
    device_ = builder.WithQueues(GetPhysicalDevice()->GetQueueFamilies())
      .Build();
    // clang-format on
    ASSERT_INITIALIZED(device_);
    Publish<DeviceInitEvent>();
  }
  InitCommandPool();
  InitDescriptorPool();
}

Driver::~Driver() {
  vkDestroyDescriptorPool(*GetDevice(), descriptor_pool_, GetAllocator());
  delete command_pool_;
  delete surface_;
  delete device_;
  delete physical_device_;
  delete instance_;
}

void Driver::CreateSemaphore(VkSemaphore& semaphore, const VkSemaphoreCreateFlags flags) {
  VkSemaphoreCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  create_info.flags = flags;
  CHECK_VK(FATAL, vkCreateSemaphore(*GetDevice(), &create_info, GetAllocator(), &semaphore),
           "failed to create VkSemaphore");
}

void Driver::CreateFence(VkFence& fence, const VkFenceCreateFlags flags) {
  VkFenceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  create_info.flags = flags;
  CHECK_VK(FATAL, vkCreateFence(*GetDevice(), &create_info, GetAllocator(), &fence), "failed to create VkFence");
}

void Driver::CreateGraphicsPipeline(const VkGraphicsPipelineCreateInfo& create_info, const VkPipelineCache& cache,
                                    VkPipeline* result) {
  const vk::Result status = vkCreateGraphicsPipelines(*GetDevice(), cache, 1, &create_info, GetAllocator(), result);
  LOG_IF(FATAL, !status) << "failed to create VkPipeline: " << status;
}

void Driver::Destroy(const VkDeviceMemory& memory) {
  vkFreeMemory(*GetDevice(), memory, GetAllocator());
}

void Driver::CreateInstance(const VkInstanceCreateInfo* create_info, VkInstance* result) {
  const auto status = vk::Result(vkCreateInstance(create_info, GetAllocator(), result));
  LOG_IF(FATAL, !status) << "failed to create VkInstance: " << status;
}

void Driver::CreateDevice(const VkDeviceCreateInfo* create_info, VkDevice* result) {
  const auto status = vk::Result(vkCreateDevice(*GetPhysicalDevice(), create_info, GetAllocator(), result));
  LOG_IF(FATAL, !status) << "failed to create VkDevice: " << status;
}

template <typename F, typename... Args>
static inline auto CallVkIfExists(const VkInstance& instance, const char* name, Args... args) -> VkResult {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  const auto func = (F)vkGetInstanceProcAddr(instance, name);
  return func ? func(instance, args...) : VK_ERROR_EXTENSION_NOT_PRESENT;
}

template <typename F, typename... Args>
static inline void InvokeVkIfExists(const VkInstance& instance, const char* name, Args... args) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  const auto func = (F)vkGetInstanceProcAddr(instance, name);
  if (func)
    func(instance, args...);
}

#ifdef PRT_DEBUG
static constexpr const auto kCreateDebugUtilsMessengerEXTName = "vkCreateDebugUtilsMessengerEXT";
VKAPI_ATTR auto VKAPI_CALL CreateDebugUtilsMessengerEXT(const VkInstance& instance,
                                                        const VkDebugUtilsMessengerCreateInfoEXT* create_info,
                                                        const VkAllocationCallbacks* allocator,
                                                        VkDebugUtilsMessengerEXT* messenger) -> VkResult {
  const auto result = CallVkIfExists<PFN_vkCreateDebugUtilsMessengerEXT>(instance, kCreateDebugUtilsMessengerEXTName,
                                                                         create_info, allocator, messenger);
  if (result == VK_ERROR_EXTENSION_NOT_PRESENT) {
    DLOG(WARNING) << "vk extension `" << kCreateDebugUtilsMessengerEXTName
                  << "` is not present, skipping debug messenger.";
    return VK_SUCCESS;
  }
  return result;
}

static constexpr const auto kDestroyDebugUtilsMessengerEXTName = "vkDestroyDebugUtilsMessengerEXT";
VKAPI_ATTR void VKAPI_CALL DestroyDebugUtilsMessengerEXT(const VkInstance& instance,
                                                         const VkDebugUtilsMessengerEXT messenger,
                                                         const VkAllocationCallbacks* allocator = nullptr) {
  return InvokeVkIfExists<PFN_vkDestroyDebugUtilsMessengerEXT>(instance, kDestroyDebugUtilsMessengerEXTName, messenger,
                                                               allocator);
}

void Driver::CreateDebugUtilsMessenger(const VkInstance& instance,
                                       const VkDebugUtilsMessengerCreateInfoEXT* create_info,
                                       VkDebugUtilsMessengerEXT* result) {
  const vk::Result status = CreateDebugUtilsMessengerEXT(instance, create_info, GetAllocator(), result);
  LOG_IF(FATAL, !status) << "failed to create VkDebugUtilsMessengerEXT: " << status;
}

void Driver::DestroyDebugUtilsMessenger(const VkDebugUtilsMessengerEXT& rhs) {
  if (rhs != VK_NULL_HANDLE)
    DestroyDebugUtilsMessengerEXT(*GetInstance(), rhs, GetAllocator());
}
#endif  // PRT_DEBUG

void Driver::CreateSwapchain(const VkSwapchainCreateInfoKHR* info, VkSwapchainKHR* result) {
  const auto status = vk::Result(vkCreateSwapchainKHR(*GetDevice(), info, GetAllocator(), result));
  LOG_IF(FATAL, !status) << "failed to create Swapchain: " << status;
}

#define DEFINE_CREATE(Type)                                                                            \
  void Driver::Create##Type(const Vk##Type##CreateInfo* create_info, Vk##Type* result) {               \
    const auto status = vk::Result(vkCreate##Type(*GetDevice(), create_info, GetAllocator(), result)); \
    LOG_IF(FATAL, !status) << "failed to create " << #Type << ": " << status;                          \
  }
FOR_EACH_VK_HANDLE(DEFINE_CREATE)
#undef DEFINE_CREATE

void Driver::DestroySwapchain(const VkSwapchainKHR& rhs) {
  if (rhs != VK_NULL_HANDLE)
    vkDestroySwapchainKHR(*GetDevice(), rhs, GetAllocator());
}

void Driver::DestroyInstance(const VkInstance& rhs) {
  if (rhs != VK_NULL_HANDLE)
    vkDestroyInstance(rhs, GetAllocator());
}

void Driver::DestroyPipeline(const VkPipeline& rhs) {
  if (rhs != VK_NULL_HANDLE)
    vkDestroyPipeline(*GetDevice(), rhs, GetAllocator());
}

#define DEFINE_DESTROY(Type)                                \
  void Driver::Destroy##Type(const Vk##Type& value) {       \
    if (value != VK_NULL_HANDLE)                            \
      vkDestroy##Type(*GetDevice(), value, GetAllocator()); \
  }
FOR_EACH_VK_HANDLE(DEFINE_DESTROY)
#undef DEFINE_FINALIZE
}  // namespace prt