#include "prette/gfx.h"
#include "prette/prette.h"

namespace prt::gfx {
  void Init() {
#ifndef OS_IS_OSX
    const auto status = glewInit();
    if(status != GLEW_OK)
      LOG(FATAL) << "failed to initialize GLEW.";
#endif //OS_IS_OSX
    glfwSetErrorCallback(&prt::OnGlfwError);
    LOG_IF(FATAL, !glfwInit()) << "error initializing GLFW";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, PRT_GLFW_CTX_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, PRT_GLFW_CTX_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef OS_IS_OSX
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  }

  void Clear(const GLbitfield mask) {
    glClear(mask);
    CHECK_GL;
  }
}