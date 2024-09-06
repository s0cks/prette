#include "prette/program/program_json.h"
#include <glog/logging.h>
#include "prette/shader/shader_vertex.h"
#include "prette/shader/shader_fragment.h"

namespace prt::program {
  Shader* ProgramShader::GetShader() const {
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

  bool ProgramReaderHandler::String(const char* value, const rapidjson::SizeType length, const bool b) {
    switch(GetState()) {
      case kVertexShader: {
        uri::Uri uri;
        if(!uri::TryParseUri(uri, value, "shader")) {
          DLOG(ERROR) << "failed to parse fragment shader uri: " << value;
          return TransitionTo(kError);
        }
        return OnParseProgramShaderRef(shader::kVertexShader, uri);
      }
      case kFragmentShader: {
        uri::Uri uri;
        if(!uri::TryParseUri(uri, value, "shader")) {
          DLOG(ERROR) << "failed to parse fragment shader uri: " << value;
          return TransitionTo(kError);
        }
        return OnParseProgramShaderRef(shader::kFragmentShader, uri);
      }
      case kGeometryShader: {
        uri::Uri uri;
        if(!uri::TryParseUri(uri, value, "shader")) {
          DLOG(ERROR) << "failed to parse fragment shader uri: " << value;
          return TransitionTo(kError);
        }
        return OnParseProgramShaderRef(shader::kGeometryShader, uri);
      }
      case kTessEvalShader: {
        uri::Uri uri;
        if(!uri::TryParseUri(uri, value, "shader")) {
          DLOG(ERROR) << "failed to parse fragment shader uri: " << value;
          return TransitionTo(kError);
        }
        return OnParseProgramShaderRef(shader::kTessEvalShader, uri);
      }
      case kTessControlShader: {
        uri::Uri uri;
        if(!uri::TryParseUri(uri, value, "shader")) {
          DLOG(ERROR) << "failed to parse fragment shader uri: " << value;
          return TransitionTo(kError);
        }
        return OnParseProgramShaderRef(shader::kTessControlShader, uri);
      }
      default:
        return json::ReaderHandlerTemplate<ProgramReaderState, ProgramReaderHandler>::String(value, length, b);
    }
  }

  bool ProgramReaderHandler::OnParseDataField(const std::string& name) {
    if(EqualsIgnoreCase(name, "fragment")) {
      return TransitionTo(kFragmentShader);
    } else if(EqualsIgnoreCase(name, "vertex")) {
      return TransitionTo(kVertexShader);
    } else if(EqualsIgnoreCase(name, "geometry")) {
      return TransitionTo(kGeometryShader);
    } else if(EqualsIgnoreCase(name, "tesseval")) {
      return TransitionTo(kTessEvalShader);
    } else if(EqualsIgnoreCase(name, "tesscontrol")) {
      return TransitionTo(kTessControlShader);
    } else if(EqualsIgnoreCase(name, "include")) {
      return TransitionTo(kIncludedShaders);
    }
    
    DLOG(ERROR) << "unexpected field: " << name;
    return TransitionTo(kError);
  }

  bool ProgramReaderHandler::OnParseProgramShaderRef(const shader::ShaderType type, const uri::Uri& uri) {
    const auto shader = ProgramShader(type, uri);
    shaders_.push_back(shader);
    return TransitionTo(kData);
  }
}