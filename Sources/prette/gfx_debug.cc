#include "prette/gfx_debug.h"
#include "prette/gfx.h"

namespace prt::gfx {
#ifndef PRT_DEBUG
  DEFINE_bool(enable_gfx_debug, false, "Enable OpenGL debugging.");
#endif //PRT_DEBUG

  void EnableDebug() {
#ifndef PRT_DEBUG
    if(!IsDebugEnabled())
      return;
#endif //PRT_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
  }

  bool IsDebugEnabled() {
#ifdef PRT_DEBUG
    return true;
#else
    return FLAGS_enable_gfx_debug;
#endif //PRT_DEBUG
  }
}