#include "prette/mouse/cursor_json.h"

namespace prt::mouse {
  auto CursorReaderHandler::String(const char* value, const rapidjson::SizeType length, const bool b) -> bool {
    switch(GetState()) {
      case kImage: {
        const std::string raw_uri(value, length);

        uri::Uri uri;
        if(!uri::Uri::TryParse(uri, raw_uri, "img")) {
          LOG(ERROR) << "failed to parse Cursor image uri: " << raw_uri;
          return TransitionTo(kError);
        }
        return OnParseImage(uri);
      }
      default: return ReaderHandlerTemplate::String(value, length, b);
    }
  }

  auto CursorReaderHandler::OnParseDataField(const std::string& name) -> bool {
    if(EqualsIgnoreCase(name, "image"))
      return TransitionTo(kImage);
    DLOG(ERROR) << "unexpected field: " << name;
    return TransitionTo(kError);
  }

  auto CursorReaderHandler::OnParseImage(const uri::Uri& uri) -> bool {
    DLOG(INFO) << "parsed Cursor image: " << uri;
    return TransitionTo(kData);
  }
}