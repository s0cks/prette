#include "prette/shader/shader.h"

namespace prt::shader {
  const std::set<std::string> TessEvalShader::kValidExtensions = {
    "json",
    "glsl",
    "tese",
  };
  
  std::string TessEvalShader::ToString() const {
    std::stringstream ss;
    ss << "TessEvalShader(";
    ss << "id=" << GetId();
    ss << ")";
    return ss.str();
  }
}