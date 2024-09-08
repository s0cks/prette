#include "prette/window/monitor.h"
#include "prette/window/monitor_printer.h"

namespace prt::window {
  auto Monitor::ToString() const -> std::string {
    std::stringstream ss;
    ss << "Monitor(";
    ss << "name=" << GetName() << ", ";
    ss << "resolution=" << GetResolution() << ", ";
    ss << "refresh_rate=" << GetRefreshRateInHertz();
    ss << ")";
    return ss.str();
  }

#ifdef PRT_DEBUG
  void PrintAllMonitors(const google::LogSeverity severity,
                        const char* file,
                        const int line,
                        const int indent) {
    MonitorPrinter printer(severity, file, line, indent);
    VisitAllMonitors(&printer);
  }
#endif //PRT_DEBUG
}