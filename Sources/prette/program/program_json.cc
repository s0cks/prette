#include "prette/program/program_json.h"
#include <glog/logging.h>
#include "prette/shader/shader_vertex.h"
#include "prette/shader/shader_fragment.h"

namespace prt::program {
  auto ProgramShader::GetShader() const -> Shader* {
    switch(GetType()) {
      case shader::kVertexShader:
        return VertexShader::New(GetUri());
      case shader::kFragmentShader:
        return FragmentShader::New(GetUri());
      default:
        LOG(FATAL) << "invalid shader type: " << GetType();
        return nullptr;
    }
  }

  auto ProgramReaderHandler::String(const char* value, const rapidjson::SizeType length, const bool b) -> bool {
    switch(GetState()) {
#define DEFINE_TRY_PARSE_SHADER_URI(Name, Ext, GlValue)                       \
      case k##Name##Shader: {                                                 \
        uri::Uri uri;                                                         \
        if(!uri::TryParseUri(uri, value, "shader")) {                         \
          DLOG(ERROR) << "failed to parse fragment shader uri: " << value;    \
          return TransitionTo(kError);                                        \
        }                                                                     \
        return OnParseProgramShaderRef(shader::k##Name##Shader, uri);         \
      }
      FOR_EACH_SHADER_TYPE(DEFINE_TRY_PARSE_SHADER_URI)
#undef DEFINE_TRY_PARSE_SHADER_URI
      default:
        return json::ReaderHandlerTemplate<ProgramReaderState, ProgramReaderHandler>::String(value, length, b);
    }
  }

  auto ProgramReaderHandler::OnParseDataField(const std::string& name) -> bool {
    if(EqualsIgnoreCase(name, "include")) {
      return TransitionTo(kIncludedShaders);
    }
#define DEFINE_TRY_PARSE_SHADER_FIELD(Name, Ext, GlValue)   \
    else if(EqualsIgnoreCase(name, #Name))                  \
      return TransitionTo(k##Name##Shader);
    FOR_EACH_SHADER_TYPE(DEFINE_TRY_PARSE_SHADER_FIELD)
#undef DEFINE_TRY_PARSE_SHADER_FIELD
    DLOG(ERROR) << "unexpected field: " << name;
    return TransitionTo(kError);
  }

  auto ProgramReaderHandler::OnParseProgramShaderRef(const shader::ShaderType type, const uri::Uri& uri) -> bool {
    const auto shader = ProgramShader(type, uri);
    shaders_.push_back(shader);
    return TransitionTo(kData);
  }
}