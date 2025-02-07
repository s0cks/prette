#ifndef PRT_RUNTIME_H
#define PRT_RUNTIME_H

#include <glog/logging.h>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/gfx.h"

namespace prt {
#ifdef PRT_DEBUG

void PrintRuntimeInfo(const google::LogSeverity severity = google::INFO, const char* file = __FILE__, const int line = __LINE__,
                      const int indent = 0);

#endif  // PRT_DEBUG

class Window;
class Runtime {
  DEFINE_NON_INSTANTIABLE_TYPE(Runtime);

 private:
  static void OnShutdown(uv_async_t* handle);
  static void InitInstance(VkInstance& instance);
  static void InitWindowSurface(VkInstance& instance, Window* window);
  static void InitPhysicalDevice(const VkInstance& instance, const VkSurfaceKHR& surface, VkPhysicalDevice& device);
  static void InitLogicalDevice(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface, VkDevice& device,
                                VkQueue& graphics_queue, VkQueue& present_queue, const float priority,
                                const std::vector<const char*>& validation_layers);
  static void OnUnhandledException();

  static void DestroyInstance(const VkAllocationCallbacks* allocator = nullptr);
  static void DestroyDevice(const VkAllocationCallbacks* allocator = nullptr);

 public:
  static auto GetVkApplicationInfo() -> const VkApplicationInfo&;
  static auto GetVkInstance() -> const VkInstance&;
  static auto GetVkPhysicalDevice() -> const VkPhysicalDevice&;
  static auto GetVkLogicalDevice() -> const VkDevice&;
  static auto GetVkGraphicsQueue() -> const VkQueue&;
  static auto GetVkPresentQueue() -> const VkQueue&;

#ifdef PRT_DEBUG
  static auto GetVkDebugUtilsMessengerEXT() -> const VkDebugUtilsMessengerEXT&;
#endif  // PRT_DEBUG

  static void Init(int argc, char** argv);
  static auto Run() -> int;
  static void Shutdown();
};
}  // namespace prt

#endif  // PRT_RUNTIME_H