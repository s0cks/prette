#include "prette/texture/texture_2d/texture_2d_json.h"

namespace prt::texture {
  bool Texture2DReaderHandler::String(const char* value, const rapidjson::SizeType length, const bool b) {
    switch(GetState()) {
      default:
        return ParentType::String(value, length, b);
    }
  }

  bool Texture2DReaderHandler::OnParseDataField(const std::string& name) {
    
    DLOG(ERROR) << "unexpected field: " << name;
    return TransitionTo(kError);
  }
}