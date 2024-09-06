#include "prette/shader/shader_tess_control.h"

namespace prt::shader {
  const ExtensionSet TessControlShader::kValidExtensions = {
    "json",
    "glsl",
    "tesc",
  };

  auto TessControlShader::ToString() const -> std::string {
    std::stringstream ss;
    ss << "TessControlShader(";
    ss << "id=" << GetId();
    ss << ")";
    return ss.str();
  }
}