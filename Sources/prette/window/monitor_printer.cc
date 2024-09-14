#ifdef PRT_DEBUG
#include "prette/window/monitor_printer.h"

namespace prt::window {
#define __ google::LogMessage(GetFile(), GetLine(), GetSeverity()).stream() << GetIndentString()
  auto MonitorPrinter::VisitMonitor(Monitor* monitor) -> bool {
    PRT_ASSERT(monitor);
    __ << monitor->ToString();
    return true;
  }
#undef __
}
#endif //PRT_DEBUG