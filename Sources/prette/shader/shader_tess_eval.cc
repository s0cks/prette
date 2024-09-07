#include "prette/shader/shader_tess_eval.h"

namespace prt::shader {
  const ExtensionSet TessEvalShader::kValidExtensions = {
    "json",
    "glsl",
    "tese",
  };
  
  auto TessEvalShader::ToString() const -> std::string {
    std::stringstream ss;
    ss << "TessEvalShader(";
    ss << "id=" << GetId() << ", ";
    ss << "meta=" << GetMeta();
    ss << ")";
    return ss.str();
  }
}