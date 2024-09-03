#ifndef PRT_SHADER_TYPE_H
#define PRT_SHADER_TYPE_H

#include <optional>

#include "prette/gfx.h"
#include "prette/json.h"

namespace prt::shader {
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

  static inline std::optional<ShaderType>
  ParseShaderType(const std::string& value) {
    if(value.empty())
      return std::nullopt;
#define DEFINE_PARSE(Name, Ext, GlValue) \
    else if(EqualsIgnoreCase(#Name, value) || EqualsIgnoreCase(#Ext, value)) return { k##Name##Shader };
    FOR_EACH_SHADER_TYPE(DEFINE_PARSE)
#undef DEFINE_PARSE
    return std::nullopt;
  }

  static inline std::optional<ShaderType>
  ParseShaderType(const json::Value& value) {
    if(!value.IsString())
      return std::nullopt;
    std::string type(value.GetString(), value.GetStringLength());
    return ParseShaderType(type);
  }

  static inline std::ostream& operator<<(std::ostream& stream, const ShaderType& rhs) {
    switch(rhs) {
#define DEFINE_TOSTRING(Name, Ext, GlValue) \
      case ShaderType::k##Name##Shader: return stream << #Name;
      FOR_EACH_SHADER_TYPE(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
      default: return stream << "Unknown Shader Type";
    }
  }

  static inline const char*
  GetExtensionForType(const ShaderType& type) {
    switch(type) {
#define DEFINE_GET_EXTENSION(Name, Ext, GlValue) \
      case ShaderType::k##Name##Shader: return (#Ext);
      FOR_EACH_SHADER_TYPE(DEFINE_GET_EXTENSION)
#undef DEFINE_GET_EXTENSION
      default: return ".glsl";
    }
  }

  static inline std::optional<ShaderType>
  DetectShaderTypeFromExtension(const std::string& extension) {
    if(extension.empty())
      return std::nullopt;
#define DETECT_SHADER_TYPE(Name, Ext, GlValue) \
    else if(EqualsIgnoreCase(extension, (#Ext))) return { k##Name##Shader };
    FOR_EACH_SHADER_TYPE(DETECT_SHADER_TYPE)
#undef DETECT_SHADER_TYPE
    return std::nullopt;
  }

  static inline std::optional<ShaderType>
  DetectShaderTypeFromExtension(const std::optional<std::string>& extension) {
    return extension ? DetectShaderTypeFromExtension(*extension) : std::nullopt;
  }
}

#endif //PRT_SHADER_TYPE_H