#ifndef PRT_SHADER_TYPE_H
#define PRT_SHADER_TYPE_H

#include <optional>
#include "prette/gfx.h"

namespace prt::shader {
  //TODO: cleanup this macro
#define FOR_EACH_SHADER_TYPE(V)                             \
  V(Vertex, vert, GL_VERTEX_SHADER)                         \
  V(Fragment, frag, GL_FRAGMENT_SHADER)                     \
  V(Geometry, geom, GL_GEOMETRY_SHADER)                     \
  V(TessControl, tesc, GL_TESS_EVALUATION_SHADER)           \
  V(TessEval, tese, GL_TESS_CONTROL_SHADER)

  class Shader;
#define FORWARD_DECLARE(Name, Ext, GlValue) class Name##Shader;
  FOR_EACH_SHADER_TYPE(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  enum ShaderType : GLenum {
#define DEFINE_SHADER_TYPE(Name, Ext, GlValue) k##Name##Shader = (GlValue),
    FOR_EACH_SHADER_TYPE(DEFINE_SHADER_TYPE)
#undef DEFINE_SHADER_TYPE

    kProgramShader,
    kNumberOfShaderTypes,
  };

  static inline auto
  ParseShaderType(const std::string& value) -> std::optional<ShaderType> {
    if(value.empty())
      return std::nullopt;
#define DEFINE_PARSE(Name, Ext, GlValue) \
    else if(EqualsIgnoreCase(#Name, value) || EqualsIgnoreCase(#Ext, value)) return { k##Name##Shader };
    FOR_EACH_SHADER_TYPE(DEFINE_PARSE)
#undef DEFINE_PARSE
    return std::nullopt;
  }

  static inline auto
  operator<<(std::ostream& stream, const ShaderType& rhs) -> std::ostream& {
    switch(rhs) {
#define DEFINE_TOSTRING(Name, Ext, GlValue) \
      case ShaderType::k##Name##Shader: return stream << #Name;
      FOR_EACH_SHADER_TYPE(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
      default: return stream << "Unknown Shader Type";
    }
  }

  static inline auto
  GetExtensionForType(const ShaderType& type) -> const char* {
    switch(type) {
#define DEFINE_GET_EXTENSION(Name, Ext, GlValue) \
      case ShaderType::k##Name##Shader: return (#Ext);
      FOR_EACH_SHADER_TYPE(DEFINE_GET_EXTENSION)
#undef DEFINE_GET_EXTENSION
      default: return ".glsl";
    }
  }

  static inline auto
  DetectShaderTypeFromExtension(const std::string& extension) -> std::optional<ShaderType> {
    if(extension.empty())
      return std::nullopt;
#define DETECT_SHADER_TYPE(Name, Ext, GlValue) \
    else if(EqualsIgnoreCase(extension, (#Ext))) return { k##Name##Shader };
    FOR_EACH_SHADER_TYPE(DETECT_SHADER_TYPE)
#undef DETECT_SHADER_TYPE
    return std::nullopt;
  }

  static inline auto
  DetectShaderTypeFromExtension(const std::optional<std::string>& extension) -> std::optional<ShaderType> {
    return extension ? DetectShaderTypeFromExtension(*extension) : std::nullopt;
  }
}

#endif //PRT_SHADER_TYPE_H