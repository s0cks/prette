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
    typedef GLFWmonitor Handle;
  private:
    const GLFWvidmode* GetVideoMode() const;
#else
#error "Unsupported Platform."
#endif //PRT_GLFW
  protected:
    Handle* handle_;

    explicit Monitor(Handle* handle);

    inline Handle* GetHandle() const {
      return handle_;
    }
  public:
    ~Monitor();
    int GetRedBits() const;
    int GetBlueBits() const;
    int GetGreenBits() const;
    int GetRefreshRateInHertz() const;
    Resolution GetResolution() const;
    std::string GetName() const;
  };
}

#endif //PRT_MONITOR_H