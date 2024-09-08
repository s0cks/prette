#include "prette/window/monitor.h"
#ifdef PRT_GLFW

namespace prt::window {
  auto Monitor::GetVideoMode() const -> const GLFWvidmode* {
    return glfwGetVideoMode(GetHandle());
  }

  auto Monitor::GetResolution() const -> Resolution {
    const auto mode = GetVideoMode();
    return {mode->width, mode->height};
  }

  auto Monitor::GetRedBits() const -> int {
    const auto mode = GetVideoMode();
    return mode->redBits;
  }

  auto Monitor::GetBlueBits() const -> int {
    const auto mode = GetVideoMode();
    return mode->blueBits;
  }

  auto Monitor::GetGreenBits() const -> int {
    const auto mode = GetVideoMode();
    return mode->greenBits;
  }

  auto Monitor::GetRefreshRateInHertz() const -> int {
    const auto mode = GetVideoMode();
    return mode->refreshRate;
  }

  auto Monitor::GetName() const -> std::string {
    const auto name = glfwGetMonitorName(GetHandle());
    return {name};
  }
}

#endif //PRT_GLFW