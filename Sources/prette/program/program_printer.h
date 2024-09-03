#ifndef PRT_PROGRAM_PRINTER_H
#define PRT_PROGRAM_PRINTER_H

#ifdef PRT_DEBUG

#include "prette/pretty_logger.h"
#include "prette/program/program.h"

namespace prt::program {
  class ProgramPrinter : public PrettyLogger,
                         public ProgramVisitor,
                         public AttributeVisitor,
                         public UniformVisitor {
  public:
    ProgramPrinter(const google::LogSeverity severity,
                   const char* file,
                   int line,
                   const int indent = 0):
      PrettyLogger(severity, file, line, indent) {
    }
    explicit ProgramPrinter(PrettyLogger* parent):
      PrettyLogger(parent) {
    }
    ~ProgramPrinter() override = default;

    bool VisitProgram(Program* program) override;
    bool VisitAttribute(const ProgramAttribute& attr) override;
    bool VisitUniform(const ProgramUniform& uniform) override;
  public:
    template<const google::LogSeverity Severity = google::INFO>
    static inline void
    Print(Program* program,
          const char* file,
          const int line,
          const int indent) {
      PRT_ASSERT(program);
      ProgramPrinter printer(Severity, file, line, indent);
      LOG_IF(ERROR, !program->Accept(&printer)) << "failed to print: " << program->ToString();
    }
  };
}

#define _PRINT_PROGRAM_(Severity, Program, File, Line, Indent)    \
  ProgramPrinter::Print<google::Severity>((Program), (File), (Line), (Indent))

#define PRINT_PROGRAM(Severity, Program)    \
  _PRINT_PROGRAM_(Severity, Program, __FILE__, __LINE__, 0)

#else

#define PRINT_PROGRAM(Severity, Program)  \

#endif //PRT_DEBUG
#endif //PRT_PROGRAM_PRINTER_H