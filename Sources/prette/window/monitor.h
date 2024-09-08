#ifndef PRT_MONITOR_H
#define PRT_MONITOR_H

#include <units.h>
#include "prette/gfx.h"
#include "prette/shape.h"
#include "prette/object.h"
#include "prette/common.h"
#include "prette/dimension.h"
#include "prette/resolution.h"

#include "prette/window/monitor_events.h"

namespace prt::window {
  class Monitor;
  class MonitorVisitor {
    DEFINE_NON_COPYABLE_TYPE(MonitorVisitor);
  protected:
    MonitorVisitor() = default;
  public:
    virtual ~MonitorVisitor() = default;
    virtual auto VisitMonitor(Monitor* rhs) -> bool = 0;
  };

  class Monitor : public Object,
                  public MonitorEventSource {
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
#endif //PRT_GLFW
  private:
    MonitorEventSubject events_;
    Handle* handle_;

    explicit Monitor(Handle* handle);

    inline auto GetHandle() const -> Handle* {
      return handle_;
    }

    void PublishEvent(MonitorEvent* event) override;
  public:
    ~Monitor() override;
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
    auto ToString() const -> std::string override;
    auto OnEvent() const -> MonitorEventObservable override;
  private:
    static auto New(Handle* handle) -> Monitor*;
  public:
    static void Init();
    static auto Get(Handle* handle) -> Monitor*;
  };

  auto GetTotalNumberOfMonitors() -> uword;
  auto GetPrimaryMonitor() -> Monitor*;
  auto VisitAllMonitors(MonitorVisitor* vis) -> bool;

#ifdef PRT_DEBUG
  void PrintAllMonitors(const google::LogSeverity severity = google::INFO,
                        const char* file = __FILE__,
                        const int line = __LINE__,
                        const int indent = 0);
#endif //PRT_DEBUG
}

#endif //PRT_MONITOR_H