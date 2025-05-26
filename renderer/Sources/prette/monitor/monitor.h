#ifndef PRT_MONITOR_H
#define PRT_MONITOR_H

#include <string>
#include <units.h>

#include "prette/common.h"
#include "prette/dimension.h"
#include "prette/event.h"
#include "prette/geometry/rectangle.h"
#include "prette/geometry/shape.h"
#include "prette/gfx.h"
#include "prette/glm.h"
#include "prette/resolution.h"
#include "prette/rx.h"

namespace prt {
#define FOR_EACH_MONITOR_EVENT(V) \
  V(MonitorConnected)             \
  V(MonitorDisconnected)

#define FORWARD_DECLARE_MONITOR_EVENT(Name) class Name##Event;
FOR_EACH_MONITOR_EVENT(FORWARD_DECLARE_MONITOR_EVENT)
#undef FORWARD_DECLARE_MONITOR_EVENT

class Monitor;
class MonitorEvent : public Event {
  DEFINE_NON_COPYABLE_TYPE(MonitorEvent);

 private:
  const Monitor* monitor_;

 protected:
  explicit MonitorEvent(const Monitor* monitor) :
    monitor_(monitor) {
    ASSERT(monitor_);
  }

 public:
  virtual ~MonitorEvent() = default;

  auto GetMonitor() const -> const Monitor* {
    return monitor_;
  }

  DEFINE_EVENT_PROTOTYPE_TYPE(Monitor, FOR_EACH_MONITOR_EVENT);
};

#define DEFINE_MONITOR_EVENT(Name)        \
  friend class Monitor;                   \
  friend class EventSource<MonitorEvent>; \
  DECLARE_EVENT_TYPE(MonitorEvent, Name);

class MonitorConnectedEvent : public MonitorEvent {
 private:
  explicit MonitorConnectedEvent(const Monitor* monitor) :
    MonitorEvent(monitor) {}

 public:
  ~MonitorConnectedEvent() override = default;
  DEFINE_MONITOR_EVENT(MonitorConnected);
};

class MonitorDisconnectedEvent : public MonitorEvent {
 private:
  explicit MonitorDisconnectedEvent(const Monitor* monitor) :
    MonitorEvent(monitor) {}

 public:
  ~MonitorDisconnectedEvent() override = default;
  DEFINE_MONITOR_EVENT(MonitorDisconnected);
};

using MonitorEventSubject = rx::subject<MonitorEvent*>;

#define DECLARE_EVENT_OBSERVABLE(Name) using Name##EventObservable = rx::observable<Name##Event*>;
DECLARE_EVENT_OBSERVABLE(Monitor);
FOR_EACH_MONITOR_EVENT(DECLARE_EVENT_OBSERVABLE);
#undef DECLARE_EVENT_OBSERVABLE

class Monitor : public EventSource<MonitorEvent> {
  friend class WindowBuilder;
  DEFINE_NON_COPYABLE_TYPE(Monitor);
#ifdef PRT_GLFW
 public:
  using Handle = GLFWmonitor;

 private:
  static void OnMonitorEvent(Handle* handle, const int event);
  auto GetVideoMode() const -> const GLFWvidmode*;
#else
#error "Unsupported Platform."
#endif  // PRT_GLFW
 private:
  Handle* handle_;
  MonitorEventSubject events_;

  explicit Monitor(Handle* handle);

  inline auto GetHandle() const -> Handle* {
    return handle_;
  }

  void PublishEvent(MonitorEvent* event) const override;

#define DEFINE_PUBLISH_EVENT(Name)           \
  inline void Publish##Name##Event() const { \
    return Publish<Name##Event>(this);       \
  }
  FOR_EACH_MONITOR_EVENT(DEFINE_PUBLISH_EVENT)
#undef DEFINE_PUBLISH_EVENT

 public:
  ~Monitor() override = default;
  auto GetRedBits() const -> int;
  auto GetBlueBits() const -> int;
  auto GetGreenBits() const -> int;
  auto GetRefreshRateInHertz() const -> int;
  auto GetResolution() const -> Resolution;
  auto GetName() const -> std::string;
  auto GetContentScale() const -> glm::vec2;
  auto GetPhysicalSize() const -> Dimension;
  auto GetPos() const -> Point;
  auto GetWorkArea() const -> Rectangle;
  auto ToString() const -> std::string;
  auto OnEvent() const -> MonitorEventObservable override;

  auto GetSubject() const -> const MonitorEventSubject& {
    return events_;
  }

#define DECLARE_ON_MONITOR_EVENT(Name)                                   \
  inline auto On##Name##Event() const->Name##EventObservable {           \
    return OnEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
  FOR_EACH_MONITOR_EVENT(DECLARE_ON_MONITOR_EVENT);
#undef DECLARE_ON_MONITOR_EVENT

 private:
  static auto New(Handle* handle) -> Monitor*;

 public:
  static void Init();
  static auto Get(Handle* handle) -> Monitor*;
};

auto GetTotalNumberOfMonitors() -> uword;
auto GetPrimaryMonitor() -> Monitor*;
}  // namespace prt

#endif  // PRT_MONITOR_H