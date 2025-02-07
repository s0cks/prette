#include "prette/runtime.h"

#include <units.h>
#include <uv.h>
#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <iterator>
#include <numeric>
#include <operators/rx-observe_on.hpp>
#include <subjects/rx-synchronize.hpp>
#include <vector>

#include "prette/command_pool.h"
#include "prette/engine.h"
#include "prette/flags.h"
#include "prette/gfx.h"
#include "prette/os_thread.h"
#include "prette/pipeline.h"
#include "prette/renderer.h"
#include "prette/runtime_info_printer.h"
#include "prette/signals.h"
#include "prette/swap_chain.h"
#include "prette/tick.h"
#include "prette/uv/utils.h"
#include "prette/window.h"

namespace prt {
static VkInstance instance_{};
static VkPhysicalDevice physical_device_{};
static VkDevice device_{};
static VkQueue graphics_queue_{};
static VkQueue present_queue_{};

static uv_async_t on_shutdown_{};

#ifdef PRT_DEBUG
static VkDebugUtilsMessengerEXT messenger_{};

auto Runtime::GetVkDebugUtilsMessengerEXT() -> const VkDebugUtilsMessengerEXT& {
  return messenger_;
}
#endif  // PRT_DEBUG

static const std::vector<const char*> kDeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    "VK_KHR_portability_subset",
};
// TODO: "VK_LAYER_LUNARG_api_dump"
static const std::vector<const char*> validation_layers_ = {"VK_LAYER_KHRONOS_validation"};

auto Runtime::GetVkInstance() -> const VkInstance& {
  return instance_;
}

auto Runtime::GetVkPhysicalDevice() -> const VkPhysicalDevice& {
  return physical_device_;
}

auto Runtime::GetVkLogicalDevice() -> const VkDevice& {
  return device_;
}

auto Runtime::GetVkGraphicsQueue() -> const VkQueue& {
  return graphics_queue_;
}

auto Runtime::GetVkPresentQueue() -> const VkQueue& {
  return present_queue_;
}

#ifdef PRT_DEBUG
void PrintRuntimeInfo(const google::LogSeverity s, const char* file, const int line, const int indent) {
  RuntimeInfoPrinter printer(s, file, line, indent);
  return printer.Print();
}
#endif  // PRT_DEBUG

void Runtime::OnUnhandledException() {
  CrashReportCause cause(std::current_exception());
  CrashReport report(cause);
  report.Print();
  LOG(FATAL) << "unhandled exception occured.";
}

static inline void GetRequiredExtensions(std::vector<const char*>& extensions) {
  uint32_t ext_count = 0;
  const auto glfw_exts = glfwGetRequiredInstanceExtensions(&ext_count);
  extensions.insert(std::end(extensions), glfw_exts, glfw_exts + ext_count);
#ifdef OS_IS_OSX
  extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif  // OS_IS_OSX

#ifdef PRT_DEBUG
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif  // PRT_DEBUG
}

#ifdef PRT_DEBUG
static VKAPI_ATTR auto VKAPI_CALL OnDebugCreateInfo(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                    VkDebugUtilsMessageTypeFlagsEXT type,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
    -> VkBool32 {
  LOG(ERROR) << "Validation Layer: " << callbackData->pMessage;
  return VK_FALSE;
}

template <typename F, typename... Args>
static inline auto CallVkIfExists(VkInstance& instance, const char* name, Args... args) -> VkResult {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  const auto func = (F)vkGetInstanceProcAddr(instance, name);
  return func ? func(instance, args...) : VK_ERROR_EXTENSION_NOT_PRESENT;
}

template <typename F, typename... Args>
static inline void InvokeVkIfExists(VkInstance& instance, const char* name, Args... args) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  const auto func = (F)vkGetInstanceProcAddr(instance, name);
  if (func)
    func(instance, args...);
}

static constexpr const auto kCreateDebugUtilsMessengerEXTName = "vkCreateDebugUtilsMessengerEXT";
VKAPI_ATTR auto VKAPI_CALL CreateDebugUtilsMessengerEXT(VkInstance& instance,
                                                        const VkDebugUtilsMessengerCreateInfoEXT* create_info,
                                                        const VkAllocationCallbacks* allocator,
                                                        VkDebugUtilsMessengerEXT* messenger) -> VkResult {
  const auto result = CallVkIfExists<PFN_vkCreateDebugUtilsMessengerEXT>(instance, kCreateDebugUtilsMessengerEXTName, create_info,
                                                                         allocator, messenger);
  if (result == VK_ERROR_EXTENSION_NOT_PRESENT) {
    DLOG(WARNING) << "vk extension `" << kCreateDebugUtilsMessengerEXTName << "` is not present, skipping debug messenger.";
    return VK_SUCCESS;
  }
  return result;
}

static constexpr const auto kDestroyDebugUtilsMessengerEXTName = "vkDestroyDebugUtilsMessengerEXT";
VKAPI_ATTR void VKAPI_CALL DestroyDebugUtilsMessengerEXT(VkInstance& instance, const VkDebugUtilsMessengerEXT messenger,
                                                         const VkAllocationCallbacks* allocator = nullptr) {
  return InvokeVkIfExists<PFN_vkDestroyDebugUtilsMessengerEXT>(instance, kDestroyDebugUtilsMessengerEXTName, messenger,
                                                               allocator);
}

static inline void InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& info,
                                                PFN_vkDebugUtilsMessengerCallbackEXT callback) {
  info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  info.pfnUserCallback = callback;
  info.pNext = nullptr;
}
#endif  // PRT_DEBUG

static inline void InitAppInfo(VkApplicationInfo& info) {
  info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  info.pApplicationName = "Hello World";
  info.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
  info.pEngineName = "No Engine";
  info.engineVersion = VK_MAKE_VERSION(0, 0, 0);
  info.pNext = nullptr;
  info.apiVersion = VK_API_VERSION_1_3;
}

static inline auto HasValidationLayerSupport() -> bool {
  uint32_t count = 0;
  vkEnumerateInstanceLayerProperties(&count, nullptr);

  std::vector<VkLayerProperties> available(count);
  vkEnumerateInstanceLayerProperties(&count, &available[0]);

  for (const auto& name : validation_layers_) {
    const auto pos = std::ranges::find_if(available, [name](const VkLayerProperties& props) {
      return strcmp(name, props.layerName) == 0;
    });
    if (pos == std::end(available))
      return false;
  }
  return true;
}

void Runtime::InitInstance(VkInstance& instance) {
#ifdef PRT_DEBUG
  LOG_IF(FATAL, !HasValidationLayerSupport()) << "vk validation layers requested but not available!";
#endif  // PRT_DEBUG
  std::vector<const char*> extensions;
  GetRequiredExtensions(extensions);
#ifdef PRT_DEBUG
  LOG(INFO) << "required vk extensions:";
  for (const auto& ext : extensions) LOG(INFO) << " - " << ext;
#endif  // PRT_DEBUG

  VkApplicationInfo app_info{};
  InitAppInfo(app_info);

  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;
  create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  create_info.enabledLayerCount = 0;
  create_info.enabledExtensionCount = extensions.size();
  create_info.ppEnabledExtensionNames = &extensions[0];

  VkDebugUtilsMessengerCreateInfoEXT debug_info{};
#ifdef PRT_DEBUG
  create_info.enabledLayerCount = validation_layers_.size();
  create_info.ppEnabledLayerNames = &validation_layers_[0];
  InitDebugMessengerCreateInfo(debug_info, &OnDebugCreateInfo);
  create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_info;
#else
  info.enabledLayerCount = 0;
  info.pNext = nullptr;
#endif  // PRT_DEBUG
  CHECK_VK(FATAL, vkCreateInstance(&create_info, nullptr, &instance), "failed to create vkInstance");
#ifdef PRT_DEBUG
  {
    VkDebugUtilsMessengerCreateInfoEXT debug_info{};
    InitDebugMessengerCreateInfo(debug_info, &OnDebugCreateInfo);
    CHECK_VK(FATAL, CreateDebugUtilsMessengerEXT(instance_, &debug_info, nullptr, &messenger_),
             "failed to create vk debug messenger");
  }
#endif  // PRT_DEBUG
  InitWindowSurface(instance_, GetAppWindow());
  const auto& surface = GetAppWindow()->GetSurface();
  InitPhysicalDevice(instance_, surface, physical_device_);
  InitLogicalDevice(physical_device_, surface, device_, graphics_queue_, present_queue_, 1.0f, validation_layers_);
  SwapChain::Init(physical_device_, device_, surface);
  Pipeline::Init(device_);
  CommandPool::Init(physical_device_, device_, surface);
  Pipeline::InitVertexBuffer(physical_device_, device_);
  Pipeline::InitIndexBuffer(physical_device_, device_);
  Renderer::Init(device_);
}

void Runtime::InitWindowSurface(VkInstance& instance, Window* window) {
  ASSERT(window);
  window->InitSurface(instance, nullptr);
}

static inline auto CheckDeviceExtensionSupport(const VkPhysicalDevice& device) -> bool {
  uint32_t count = 0;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);
  std::vector<VkExtensionProperties> available(count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &count, &available[0]);

  std::set<std::string> required(std::begin(kDeviceExtensions), std::end(kDeviceExtensions));
  for (const auto& ext : available) {
    required.erase(ext.extensionName);
  }
  return required.empty();
}

static inline auto IsDeviceSuitable(const VkSurfaceKHR& surface) -> std::function<bool(const VkPhysicalDevice& device)> {
  return [&surface](const VkPhysicalDevice& device) {
    QueueFamilyIndices indices = FindQueueFamilies(device, surface);
    const auto extensions_supported = CheckDeviceExtensionSupport(device);
    bool swap_supported = false;
    if (extensions_supported) {
      const auto window = GetAppWindow();
      ASSERT(window);
      swap_supported = QuerySwapChainSupport(device, window->GetSurface());
    }
    return indices.IsComplete() && extensions_supported && swap_supported;
  };
}

void Runtime::InitPhysicalDevice(const VkInstance& instance, const VkSurfaceKHR& surface, VkPhysicalDevice& device) {
  LOG_IF(FATAL, !FindSuitablePhysicalDevice(instance, &device, IsDeviceSuitable(surface)))
      << "failed to find suitable GPU w/ vulkan support.";
#ifdef PRT_DEBUG
  LOG(INFO) << "found suitable GPU w/ vulkan support:";
  PrintProperties(device);
#endif  // PRT_DEBUG
}

void Runtime::InitLogicalDevice(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface, VkDevice& device,
                                VkQueue& graphics_queue, VkQueue& present_queue, const float priority,
                                const std::vector<const char*>& validation_layers) {
  const auto indices = FindQueueFamilies(physical_device, surface);
  std::unordered_set<uint32_t> unique_families{};
  indices.GetUniqueFamilies(unique_families);
  std::vector<VkDeviceQueueCreateInfo> create_infos{};

  for (const auto& family : unique_families) {
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = family;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &priority;
    create_infos.push_back(queue_create_info);
  }

  VkPhysicalDeviceFeatures device_features{};

  VkDeviceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.pQueueCreateInfos = &create_infos[0];
  create_info.queueCreateInfoCount = create_infos.size();
  create_info.pEnabledFeatures = &device_features;
  create_info.enabledExtensionCount = kDeviceExtensions.size();
  create_info.ppEnabledExtensionNames = &kDeviceExtensions[0];

#ifdef PRT_DEBUG
  create_info.enabledLayerCount = validation_layers.size();
  create_info.ppEnabledLayerNames = &validation_layers[0];
#else
  create_info.enabledLayerCount = 0;
#endif  // PRT_DEBUG
  CHECK_VK(FATAL, vkCreateDevice(physical_device, &create_info, nullptr, &device), "failed to create vk device");
  indices.GetGraphicsQueue(device, graphics_queue);
  indices.GetPresentQueue(device, present_queue);
}

void Runtime::DestroyDevice(const VkAllocationCallbacks* allocator) {
  vkDestroyDevice(device_, allocator);
}

void Runtime::DestroyInstance(const VkAllocationCallbacks* allocator) {
  vkDestroyInstance(instance_, allocator);
}

void Runtime::OnShutdown(uv_async_t* handle) {
  DLOG(INFO) << "shutting down....";
  const auto engine = GetEngine();
  ASSERT(engine);
  engine->Shutdown();
  const auto window = GetAppWindow();
  window->Close();

  vkDeviceWaitIdle(device_);
  Renderer::Shutdown(device_);
  SwapChain::Shutdown(device_);
  CommandPool::Shutdown(device_);
  Pipeline::Shutdown(device_);
  Pipeline::DestroyIndexBuffer(device_);
  Pipeline::DestroyVertexBuffer(device_);
  DestroyDevice();
#ifdef PRT_DEBUG
  DestroyDebugUtilsMessengerEXT(instance_, messenger_);
#endif  // PRT_DEBUG
  window->DestroySurface(instance_);
  DestroyInstance();
}

void Runtime::Shutdown() {
  uv_async_send(&on_shutdown_);
  // TODO:
  //  - destroy window
  //  - terminate glfw
}

template <typename N>
static inline auto Sum(const std::vector<N>& values) -> N {
  return std::reduce(std::begin(values), std::end(values), 0);
}

template <typename N>
static inline auto Average(const std::vector<N>& values) -> double {
  const auto total = Sum(values);
  return total / values.size();
}

void Runtime::Init(int argc, char** argv) {
  srand(time(nullptr));

  // ::google::InstallPrefixFormatter(&MyPrefixFormatter);
  ::google::InitGoogleLogging(argv[0]);
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  InitSignalHandlers();
  std::set_terminate(OnUnhandledException);
  LOG_IF(FATAL, !SetCurrentThreadName("main")) << "failed to set main thread name.";
  gfx::Init();
  engine::InitEngine();

  const auto engine = GetEngine();
  ASSERT(engine);
  uv::Async::Init(engine->GetLoop(), &on_shutdown_, &OnShutdown);
#ifdef PRT_DEBUG
  engine->OnTick()
      .map([](engine::TickEvent* event) {
        return (event->GetTimeSinceLast()).value();
      })
      .buffer(1000)
      .subscribe([](std::vector<uint64_t> deltas) {
        const auto avg = Average(deltas);
        const auto min = std::ranges::min_element(deltas);
        const auto max = std::ranges::max_element(deltas);
        using ns = units::time::nanosecond_t;
        DLOG(INFO) << "tick rate: avg=" << ns(avg) << ", min=" << ns((*min)) << ", max=" << ns((*max));
      });
#endif  // PRT_DEBUG

  InitWindows();
  InitInstance(instance_);
}

auto Runtime::Run() -> int {
#ifdef PRT_DEBUG
  PrintRuntimeInfo();
#endif  // PRT_DEBUG
  const auto engine = GetEngine();
  ASSERT(engine);
  engine->Run();
  return EXIT_SUCCESS;
}
}  // namespace prt