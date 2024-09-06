#ifndef PRT_SHADER_PRINTER_H
#define PRT_SHADER_PRINTER_H

#ifdef PRT_DEBUG

#include "prette/shader/shader.h"
#include "prette/pretty_logger.h"

namespace prt::shader {
  class ShaderPrinter : public ShaderVisitor,
                        public PrettyLogger {
    DEFINE_NON_COPYABLE_TYPE(ShaderPrinter);
  public:
    ShaderPrinter(const google::LogSeverity severity,
                  const char* file,
                  int line,
                  const int indent):
      PrettyLogger(severity, file, line, indent) {  
    }
    explicit ShaderPrinter(PrettyLogger* parent):
      PrettyLogger(parent) {
    }
    ~ShaderPrinter() override = default;
#define DECLARE_VISIT_SHADER(Name, Ext, GlValue)                        \
    auto Visit##Name##Shader(Name##Shader* shader) -> bool override;
    FOR_EACH_SHADER_TYPE(DECLARE_VISIT_SHADER)
#undef DECLARE_VISIT_SHADER
  };
}

#endif //PRT_DEBUG
#endif //PRT_SHADER_PRINTER_H