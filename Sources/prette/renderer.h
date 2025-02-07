#ifndef PRT_RENDERER_H
#define PRT_RENDERER_H

#include <vector>

#include "prette/gfx.h"

namespace prt {
class Renderer {
 private:
  static void InitSyncObjects(const VkDevice& device, const VkAllocationCallbacks* allocator);
  static void InitResizeListener();
  static void DestroySyncObjects(const VkDevice& device, const VkAllocationCallbacks* allocator);

 public:
  static inline void Init(const VkDevice& device, const VkAllocationCallbacks* allocator = nullptr) {
    InitSyncObjects(device, allocator);
    InitResizeListener();
  }

  static inline void Shutdown(const VkDevice& device, const VkAllocationCallbacks* allocator = nullptr) {
    DestroySyncObjects(device, allocator);
  }

  static void DrawFrame(const VkDevice& device);
};
}  // namespace prt

#endif  // PRT_RENDERER_H
