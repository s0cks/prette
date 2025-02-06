#include "prette/runtime.h"

#include <units.h>
#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <iterator>
#include <numeric>
#include <operators/rx-observe_on.hpp>
#include <subjects/rx-synchronize.hpp>
#include <vector>

#include "prette/engine.h"
#include "prette/flags.h"
#include "prette/gfx.h"
#include "prette/os_thread.h"
#include "prette/runtime_info_printer.h"
#include "prette/signals.h"
#include "prette/tick.h"
#include "prette/window.h"

namespace prt {
static VkApplicationInfo app_info_{};
static VkInstance instance_{};
static VkPhysicalDevice physical_device_{};
static VkDevice device_{};
static VkQueue graphics_queue_{};

#ifdef PRT_DEBUG
static VkDebugUtilsMessengerEXT messenger_{};
#endif  // PRT_DEBUG

static const std::vector<const char*> kDeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};
static const std::vector<const char*> validation_layers_ = {
    "VK_LAYER_KHRONOS_validation",
};

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

void Runtime::InitApplicationInfo(VkApplicationInfo& info) {
  info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  info.pApplicationName = "Hello World";
  info.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
  info.pEngineName = "No Engine";
  info.engineVersion = VK_MAKE_VERSION(0, 0, 0);
  info.apiVersion = VK_API_VERSION_1_3;
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
static inline auto CreateDebugUtilsMessengerEXT(VkInstance& instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info,
                                                const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* messenger)
    -> VkResult {
  const auto result = CallVkIfExists<PFN_vkCreateDebugUtilsMessengerEXT>(instance, kCreateDebugUtilsMessengerEXTName, create_info,
                                                                         allocator, messenger);
  if (result == VK_ERROR_EXTENSION_NOT_PRESENT) {
    DLOG(WARNING) << "vk extension `" << kCreateDebugUtilsMessengerEXTName << "` is not present, skipping debug messenger.";
    return VK_SUCCESS;
  }
  return result;
}

static constexpr const auto kDestroyDebugUtilsMessengerEXTName = "vkDestroyDebugUtilsMessengerEXT";
static inline void DestroyDebugUtilsMessengerEXT(VkInstance& instance, const VkDebugUtilsMessengerEXT messenger,
                                                 const VkAllocationCallbacks* allocator) {
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
}
#endif  // PRT_DEBUG

static inline void InitInstanceCreateInfo(VkInstanceCreateInfo& info, const std::vector<const char*>& extensions) {
  info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  info.pApplicationInfo = &app_info_;
  info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  info.enabledLayerCount = 0;
  info.enabledExtensionCount = extensions.size();
  info.ppEnabledExtensionNames = &extensions[0];

#ifdef PRT_DEBUG
  info.enabledLayerCount = validation_layers_.size();
  info.ppEnabledLayerNames = &validation_layers_[0];
  VkDebugUtilsMessengerCreateInfoEXT debug_info{};
  InitDebugMessengerCreateInfo(debug_info, &OnDebugCreateInfo);
  info.pNext = &debug_info;
#else
  info.enabledLayerCount = 0;
  info.pNext = nullptr;
#endif  // PRT_DEBUG
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
  InitApplicationInfo(app_info_);

  std::vector<const char*> extensions;
  GetRequiredExtensions(extensions);
#ifdef PRT_DEBUG
  LOG(INFO) << "required vk extensions:";
  for (const auto& ext : extensions) LOG(INFO) << " - " << ext;
#endif  // PRT_DEBUG

  VkInstanceCreateInfo create_info{};
  InitInstanceCreateInfo(create_info, extensions);
  CHECK_VK(FATAL, vkCreateInstance(&create_info, nullptr, &instance), "failed to create vkInstance");
#ifdef PRT_DEBUG
  {
    VkDebugUtilsMessengerCreateInfoEXT debug_info{};
    InitDebugMessengerCreateInfo(debug_info, &OnDebugCreateInfo);
    CHECK_VK(FATAL, CreateDebugUtilsMessengerEXT(instance_, &debug_info, nullptr, &messenger_),
             "failed to create vk debug messenger");
  }
#endif  // PRT_DEBUG
  InitPhysicalDevice(instance_, physical_device_);
  InitLogicalDevice(device_);
}

void Runtime::InitPhysicalDevice(const VkInstance& instance, VkPhysicalDevice& device) {
  LOG_IF(FATAL, !FindSuitablePhysicalDevice(instance, &device)) << "failed to find suitable GPU w/ vulkan support.";
#ifdef PRT_DEBUG
  LOG(INFO) << "found suitable GPU w/ vulkan support:";
  PrintProperties(device);
#endif  // PRT_DEBUG
}

void Runtime::InitLogicalDevice(VkDevice& device, const float priority) {
  const auto indices = FindQueueFamilies(physical_device_);

  VkDeviceQueueCreateInfo queue_create_info{};
  queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_create_info.queueFamilyIndex = indices.graphics.value();
  queue_create_info.queueCount = 1;
  queue_create_info.pQueuePriorities = &priority;

  VkPhysicalDeviceFeatures device_features{};

  VkDeviceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.pQueueCreateInfos = &queue_create_info;
  create_info.queueCreateInfoCount = 1;
  create_info.pEnabledFeatures = &device_features;
  create_info.enabledExtensionCount = 0;

#ifdef PRT_DEBUG
  create_info.enabledLayerCount = validation_layers_.size();
  create_info.ppEnabledLayerNames = &validation_layers_[0];
#else
  create_info.enabledLayerCount = 0;
#endif  // PRT_DEBUG
  CHECK_VK(FATAL, vkCreateDevice(physical_device_, &create_info, nullptr, &device), "failed to create vk device");
  vkGetDeviceQueue(device, indices.graphics.value(), 0, &graphics_queue_);
}

void Runtime::Shutdown() {
  DLOG(INFO) << "shutting down....";
  const auto window = GetAppWindow();
  window->Close();
  const auto engine = GetEngine();
  ASSERT(engine);
  engine->Shutdown();

  vkDestroyDevice(device_, nullptr);
#ifdef PRT_DEBUG
  DestroyDebugUtilsMessengerEXT(instance_, messenger_, nullptr);
#endif  // PRT_DEBUG
  vkDestroyInstance(instance_, nullptr);
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
#ifdef PRT_DEBUG
  const auto engine = GetEngine();
  ASSERT(engine);
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