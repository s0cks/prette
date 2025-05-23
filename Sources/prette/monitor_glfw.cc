#include "prette/monitor.h"
#ifdef PRT_GLFW

#include <string>

#include "prette/common.h"
#include "prette/dimension.h"
#include "prette/geometry/rectangle.h"
#include "prette/geometry/shape.h"
#include "prette/gfx.h"
#include "prette/glm.h"
#include "prette/resolution.h"

namespace prt {
using Handle = Monitor::Handle;

template <typename T>
static inline auto GetUserPointer(Handle* handle) -> T* {
  ASSERT(handle);
  return (T*)glfwGetMonitorUserPointer(handle);
}

template <typename T>
static inline void SetUserPointer(Handle* handle, T* ptr) {
  ASSERT(handle);
  return glfwSetMonitorUserPointer(handle, ptr);
}

static inline auto HasUserPointer(Handle* handle) -> bool {
  ASSERT(handle);
  return GetUserPointer<void*>(handle) != nullptr;
}

Monitor::Monitor(Handle* handle) :
  handle_(handle) {
  SetUserPointer(handle, this);
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
  return {width, height};
}

auto Monitor::GetContentScale() const -> glm::vec2 {
  float xScale{}, yScale{};
  glfwGetMonitorContentScale(GetHandle(), &xScale, &yScale);
  return {xScale, yScale};
}

auto Monitor::GetPos() const -> Point {
  int xPos{}, yPos{};
  glfwGetMonitorPos(GetHandle(), &xPos, &yPos);
  return {xPos, yPos};
}

auto Monitor::GetWorkArea() const -> Rectangle {
  int xPos{}, yPos{}, width{}, height{};
  glfwGetMonitorWorkarea(GetHandle(), &xPos, &yPos, &width, &height);
  return {static_cast<uword>(xPos), static_cast<uword>(yPos), static_cast<uword>(width), static_cast<uword>(height)};
}

auto Monitor::OnEvent() const -> MonitorEventObservable {
  return events_.get_observable();
}

void Monitor::PublishEvent(MonitorEvent* event) const {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

auto Monitor::New(Handle* handle) -> Monitor* {
  ASSERT(!HasUserPointer(handle));
  return new Monitor(handle);
}

auto Monitor::Get(Handle* handle) -> Monitor* {
  return HasUserPointer(handle) ? GetUserPointer<Monitor>(handle) : New(handle);
}

void Monitor::OnMonitorEvent(Handle* handle, const int event) {
  const auto monitor = Monitor::Get(handle);
  ASSERT(monitor);
  switch (event) {
    case GLFW_CONNECTED:
      return monitor->PublishMonitorConnectedEvent();
    case GLFW_DISCONNECTED:
      return monitor->PublishMonitorDisconnectedEvent();
    default:
      LOG(ERROR) << "unknown glfw monitor event '" << event << "' on: " << monitor->ToString();
  }
}

void Monitor::Init() {
  glfwSetMonitorCallback(&OnMonitorEvent);
}

auto GetTotalNumberOfMonitors() -> uword {
  int count{};
  glfwGetMonitors(&count);  // ignore the returned array
  return static_cast<uword>(count);
}

auto GetPrimaryMonitor() -> Monitor* {
  return Monitor::Get(glfwGetPrimaryMonitor());
}
}  // namespace prt

#endif  // PRT_GLFW