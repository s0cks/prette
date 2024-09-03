#include "prette/shader/shader_printer.h"

namespace prt::shader {
#define DEFINE_VISIT_SHADER(Name, Ext, Value)                       \
  bool ShaderPrinter::Visit##Name##Shader(Name##Shader* shader) {   \
    PRT_ASSERT(shader);                                             \
    DLOG(INFO) << shader->ToString();                               \
    return true;                                                    \
  }
  FOR_EACH_SHADER_TYPE(DEFINE_VISIT_SHADER);
#undef DEFINE_VISIT_SHADER
}