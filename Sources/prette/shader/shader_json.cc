#include "prette/shader/shader_json.h"

namespace prt::shader {
  auto ShaderReaderHandler::String(const char* value, const rapidjson::SizeType length, const bool b) -> bool {
    switch(GetState()) {
      case kSources:
        return OnParseSource(std::string(value, length));
      default:
        return ReaderHandlerTemplate::String(value, length, b);
    }
  }

  auto ShaderReaderHandler::StartArray() -> bool {
    switch(GetState()) {
      case kSources:
        return NoTransition();
      default:
        return TransitionTo(kError);
    }
  }

  auto ShaderReaderHandler::EndArray(const json::SizeType size) -> bool {
    switch(GetState()) {
      case kSources:
        return TransitionTo(kData);
      default:
        return TransitionTo(kError);
    }
  }

  auto ShaderReaderHandler::OnParseSource(const std::string& source) -> bool {
    sources_.push_back(source);
    return NoTransition();
  }

  auto ShaderReaderHandler::OnParseDataField(const std::string& name) -> bool {
    if(EqualsIgnoreCase(name, "sources")) {
      return TransitionTo(kSources);
    }

    DLOG(ERROR) << "unexpected field: " << name;
    return TransitionTo(kError);
  }
}