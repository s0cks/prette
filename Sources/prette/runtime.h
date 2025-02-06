#ifndef PRT_RUNTIME_H
#define PRT_RUNTIME_H

#include <glog/logging.h>

#include "prette/common.h"
#include "prette/gfx.h"

namespace prt {
#ifdef PRT_DEBUG

void PrintRuntimeInfo(const google::LogSeverity severity = google::INFO, const char* file = __FILE__, const int line = __LINE__,
                      const int indent = 0);

#endif  // PRT_DEBUG

class Runtime {
  friend class RuntimeInfoPrinter;
  DEFINE_NON_INSTANTIABLE_TYPE(Runtime);

 private:
  static void InitApplicationInfo(VkApplicationInfo& info);
  static void InitInstance(VkInstance& instance);
  static void InitPhysicalDevice(const VkInstance& instance, VkPhysicalDevice& device);
  static void InitLogicalDevice(VkDevice& device, const float priority = 1.0f);
  static void OnUnhandledException();

 public:
  static void Init(int argc, char** argv);
  static auto Run() -> int;
  static void Shutdown();
};
}  // namespace prt

#endif  // PRT_RUNTIME_H