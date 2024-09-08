#ifndef PRT_MONITOR_H
#define PRT_MONITOR_H

#include <units.h>
#include "prette/gfx.h"
#include "prette/common.h"
#include "prette/resolution.h"

namespace prt::window {
  class Monitor {
    friend class WindowBuilder;
    DEFINE_NON_COPYABLE_TYPE(Monitor);
#ifdef PRT_GLFW
  public:
    using Handle = GLFWmonitor;
  private:
    auto GetVideoMode() const -> const GLFWvidmode*;
#else
#error "Unsupported Platform."
#endif //PRT_GLFW
  private:
    Handle* handle_;

    explicit Monitor(Handle* handle);

    inline auto GetHandle() const -> Handle* {
      return handle_;
    }
  public:
    ~Monitor() = default;
    auto GetRedBits() const -> int;
    auto GetBlueBits() const -> int;
    auto GetGreenBits() const -> int;
    auto GetRefreshRateInHertz() const -> int;
    auto GetResolution() const -> Resolution;
    auto GetName() const -> std::string;
  };
}

#endif //PRT_MONITOR_H