#ifndef PRT_SHADER_UNIT_PRINTER_H
#define PRT_SHADER_UNIT_PRINTER_H

#include "prette/pretty_logger.h"
#include "prette/shader/shader_unit.h"

namespace prt::shader {
  class ShaderUnitPrinter : public PrettyLogger,
                            public ShaderCodeVisitor {
  protected:
  public:
    ShaderUnitPrinter(const google::LogSeverity severity,
                      const char* file,
                      const int line,
                      const int indent):
      PrettyLogger(severity, file, line, indent) {
    }
    explicit ShaderUnitPrinter(PrettyLogger* parent):
      PrettyLogger(parent) {
    }
    ~ShaderUnitPrinter() override = default;
    bool VisitShaderCode(ShaderCode* code) override;
    void PrintShaderUnit(ShaderUnit* unit);
  public:
    template<const google::LogSeverity Severity = google::INFO>
    static inline void
    Print(ShaderUnit* unit, const char* file, const int line, int indent = 0) {
      ShaderUnitPrinter printer(Severity, file, line, indent);
      return printer.PrintShaderUnit(unit);
    }
  };
}

#endif //PRT_SHADER_UNIT_PRINTER_H