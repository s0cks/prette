#ifndef PRT_RENDERER_H
#define PRT_RENDERER_H

#include <vector>

#include "prette/gfx.h"

namespace prt {
class Renderer {
 private:
  static void InitSyncObjects(Driver* driver);
  static void InitResizeListener();
  static void DestroySyncObjects(Driver* driver);

 public:
  static inline void Init(Driver* driver) {
    ASSERT(driver);
    InitSyncObjects(driver);
    InitResizeListener();
  }

  static inline void Shutdown(Driver* driver) {
    ASSERT(driver);
    DestroySyncObjects(driver);
  }

  static void DrawFrame(Driver* driver);
};
}  // namespace prt

#endif  // PRT_RENDERER_H
