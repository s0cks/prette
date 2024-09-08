#ifndef PRT_RUNTIME_INFO_PRINTER_H
#define PRT_RUNTIME_INFO_PRINTER_H
#ifdef PRT_DEBUG

#include "prette/object.h"
#include "prette/pretty_logger.h"

namespace prt {
  class RuntimeInfoPrinter : public PrettyLogger {
    using Severity=google::LogSeverity;
    DEFINE_NON_COPYABLE_TYPE(RuntimeInfoPrinter);
  private:
    auto PrintObject(Object* obj) -> bool;
    void PrintEngineInfo();
    void PrintMouseInfo();
  public:
    explicit RuntimeInfoPrinter(const Severity severity = google::INFO,
                                const char* file = __FILE__,
                                const int line = __LINE__,
                                const int indent = 0):
      PrettyLogger(severity, file, line, indent) {
    }
    ~RuntimeInfoPrinter() override = default;

    auto GetSeverity() const -> Severity {
      return severity_;
    }

    void Print();
  };
}

#endif //PRT_DEBUG
#endif //PRT_RUNTIME_INFO_PRINTER_H