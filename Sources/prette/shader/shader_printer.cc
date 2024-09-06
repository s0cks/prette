#include "prette/shader/shader_printer.h"

#include "prette/shader/shader_vertex.h"
#include "prette/shader/shader_fragment.h"
#include "prette/shader/shader_geometry.h"
#include "prette/shader/shader_tess_eval.h"
#include "prette/shader/shader_tess_control.h"

namespace prt::shader {
#define DEFINE_VISIT_SHADER(Name, Ext, Value)                               \
  auto ShaderPrinter::Visit##Name##Shader(Name##Shader* shader) -> bool {   \
    PRT_ASSERT(shader);                                                     \
    DLOG(INFO) << shader->ToString();                                       \
    return true;                                                            \
  }
  FOR_EACH_SHADER_TYPE(DEFINE_VISIT_SHADER);
#undef DEFINE_VISIT_SHADER
}