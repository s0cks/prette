#ifndef PRT_MONITOR_PRINTER_H
#define PRT_MONITOR_PRINTER_H
#include "prette/common.h"
#ifdef PRT_DEBUG

#include "prette/pretty_logger.h"
#include "prette/window/monitor.h"

namespace prt::window {
  class MonitorPrinter : public MonitorVisitor,
                         public PrettyLogger {
    DEFINE_NON_COPYABLE_TYPE(MonitorPrinter);
  public:
    MonitorPrinter(const google::LogSeverity severity,
                   const char* file,
                   int line,
                   const int indent):
      PrettyLogger(severity, file, line, indent) {
    }
    explicit MonitorPrinter(PrettyLogger* parent):
      PrettyLogger(parent) {
    }
    ~MonitorPrinter() override = default;
    auto VisitMonitor(Monitor* monitor) -> bool override;
  };
}

#endif //PRT_DEBUG
#endif // PRT_MONITOR_PRINTER_H
