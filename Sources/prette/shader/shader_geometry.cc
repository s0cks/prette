#include "prette/shader/shader_geometry.h"

namespace prt::shader {
  const ExtensionSet GeometryShader::kValidExtensions = {
    "json",
    "glsl",
    "geom",
  };

  auto GeometryShader::ToString() const -> std::string {
    std::stringstream ss;
    ss << "GeometryShader(";
    ss << "id=" << GetId();
    ss << ")";
    return ss.str();
  }
}