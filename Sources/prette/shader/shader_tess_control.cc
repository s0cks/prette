#include "prette/shader/shader.h"

namespace prt::shader {
  const std::set<std::string> TessControlShader::kValidExtensions = {
    "json",
    "glsl",
    "tesc",
  };

  std::string TessControlShader::ToString() const {
    std::stringstream ss;
    ss << "TessControlShader(";
    ss << "id=" << GetId();
    ss << ")";
    return ss.str();
  }
}