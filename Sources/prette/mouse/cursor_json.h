#ifndef PRT_CURSOR_JSON_H
#define PRT_CURSOR_JSON_H

#include "prette/uri.h"
#include "prette/json.h"

namespace prt::mouse {
#define FOR_EACH_CURSOR_READER_STATE(V)       \
  FOR_EACH_JSON_READER_TEMPLATE_STATE(V)      \
  V(Image)

  enum CursorReaderState : uint16_t {
#define DEFINE_STATE(Name) k##Name,
    FOR_EACH_CURSOR_READER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
  };

  static inline auto
  operator<<(std::ostream& stream, const CursorReaderState& rhs) -> std::ostream& {
    switch(rhs) {
#define DEFINE_TO_STRING(Name)  \
      case k##Name: return stream << #Name;
      FOR_EACH_CURSOR_READER_STATE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return stream << "Unknown CursorReaderState: " << static_cast<uword>(rhs);
    }
  }

  class CursorReaderHandler : public json::ReaderHandlerTemplate<CursorReaderState, CursorReaderHandler> {
    static constexpr const auto kExpectedType = "Cursor";
  protected:
    auto OnParseDataField(const std::string& name) -> bool override;
    auto OnParseImage(const uri::Uri& uri) -> bool;
  public:
    explicit CursorReaderHandler():
      json::ReaderHandlerTemplate<CursorReaderState, CursorReaderHandler>(kExpectedType) {
    }
    ~CursorReaderHandler() override = default;
    auto String(const char* value, const rapidjson::SizeType length, const bool) -> bool override;
  };
}

#endif //PRT_CURSOR_JSON_H