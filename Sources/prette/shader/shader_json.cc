#include "prette/shader/shader_json.h"

namespace prt::shader {
  bool ShaderReaderHandler::String(const char* value, const rapidjson::SizeType length, const bool b) {
    switch(GetState()) {
      case kSources:
        return OnParseSource(std::string(value, length));
      default:
        return ReaderHandlerTemplate::String(value, length, b);
    }
  }

  bool ShaderReaderHandler::StartArray() {
    switch(GetState()) {
      case kSources:
        return NoTransition();
      default:
        return TransitionTo(kError);
    }
  }

  bool ShaderReaderHandler::EndArray(const json::SizeType size) {
    switch(GetState()) {
      case kSources:
        return TransitionTo(kData);
      default:
        return TransitionTo(kError);
    }
  }

  bool ShaderReaderHandler::OnParseSource(const std::string& source) {
    sources_.push_back(source);
    return NoTransition();
  }

  bool ShaderReaderHandler::OnParseDataField(const std::string& name) {
    if(EqualsIgnoreCase(name, "sources")) {
      return TransitionTo(kSources);
    }

    DLOG(ERROR) << "unexpected field: " << name;
    return TransitionTo(kError);
  }
}