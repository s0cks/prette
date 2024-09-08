#include "prette/window/monitor.h"
#include "prette/window/monitor_events.h"
#include <GLFW/glfw3.h>
#ifdef PRT_GLFW

namespace prt::window {
  using Handle = Monitor::Handle;

  template<typename T>
  static inline auto
  GetUserPointer(Handle* handle) -> T* {
    PRT_ASSERT(handle);
    return (T*) glfwGetMonitorUserPointer(handle);
  }

  template<typename T>
  static inline void
  SetUserPointer(Handle* handle, T* ptr) {
    PRT_ASSERT(handle);
    return glfwSetMonitorUserPointer(handle, ptr);
  }

  static inline auto
  HasUserPointer(Handle* handle) -> bool {
    PRT_ASSERT(handle);
    return GetUserPointer<void*>(handle) != nullptr;
  }

  Monitor::Monitor(Handle* handle):
    MonitorEventSource(),
    handle_(handle) {
    SetUserPointer(handle, this);
  }

  Monitor::~Monitor() {
    // do nothing for now
  }

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

  auto Monitor::GetPhysicalSize() const -> Dimension {
    int width{}, height{};
    glfwGetMonitorPhysicalSize(GetHandle(), &width, &height);
    return { width, height };
  }

  auto Monitor::GetContentScale() const -> glm::vec2 {
    float xScale{}, yScale{};
    glfwGetMonitorContentScale(GetHandle(), &xScale, &yScale);
    return { xScale, yScale };
  }

  auto Monitor::GetPos() const -> Point {
    int xPos{}, yPos{};
    glfwGetMonitorPos(GetHandle(), &xPos, &yPos);
    return { xPos, yPos };
  }

  auto Monitor::GetWorkArea() const -> Rectangle {
    int xPos{}, yPos{}, width{}, height{};
    glfwGetMonitorWorkarea(GetHandle(), &xPos, &yPos, &width, &height);
    return {
      static_cast<uword>(xPos),
      static_cast<uword>(yPos),
      static_cast<uword>(width),
      static_cast<uword>(height)
    };
  }

  auto Monitor::OnEvent() const -> MonitorEventObservable {
    return events_.get_observable();
  }

  void Monitor::PublishEvent(MonitorEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    return subscriber.on_next(event);
  }

  auto Monitor::New(Handle* handle) -> Monitor* {
    PRT_ASSERT(!HasUserPointer(handle));
    return new Monitor(handle);
  }

  auto Monitor::Get(Handle* handle) -> Monitor* {
    return HasUserPointer(handle)
      ? GetUserPointer<Monitor>(handle)
      : New(handle);
  }

  void Monitor::OnMonitorEvent(Handle* handle, const int event) {
    const auto monitor = Monitor::Get(handle);
    PRT_ASSERT(monitor);
    switch(event) {
      case GLFW_CONNECTED:
        return monitor->Publish<MonitorConnectedEvent>(monitor);
      case GLFW_DISCONNECTED:
        return monitor->Publish<MonitorDisconnectedEvent>(monitor);
      default:
        LOG(ERROR) << "unknown glfw monitor event '" << event << "' on: " << monitor->ToString();
    }
  }

  void Monitor::Init() {
    glfwSetMonitorCallback(&OnMonitorEvent);
  }

  auto GetTotalNumberOfMonitors() -> uword {
    int count{};
    glfwGetMonitors(&count); // ignore the returned array
    return static_cast<uword>(count);
  }

  auto GetPrimaryMonitor() -> Monitor* {
    return Monitor::Get(glfwGetPrimaryMonitor());
  }

  auto VisitAllMonitors(MonitorVisitor* vis) -> bool {
    PRT_ASSERT(vis);
    int count{};
    const auto monitors = glfwGetMonitors(&count);
    for(auto idx = 0; idx < count; idx++) {
      const auto monitor = Monitor::Get(monitors[idx]);
      if(!vis->VisitMonitor(monitor))
        return false;
    }
    return true;
  }
}

#endif //PRT_GLFW