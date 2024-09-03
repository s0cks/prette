#ifndef PRT_TEXTURE_2D_JSON_H
#define PRT_TEXTURE_2D_JSON_H

#include <ostream>
#include "prette/json.h"

namespace prt::texture {
#define FOR_EACH_TEXTURE2D_READER_STATE(V)        \
  FOR_EACH_JSON_READER_TEMPLATE_STATE(V)          \

  enum Texture2DReaderState {
#define DEFINE_STATE(Name) k##Name,
    FOR_EACH_TEXTURE2D_READER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
  };
    
  static inline std::ostream&
  operator<<(std::ostream& stream, const Texture2DReaderState& rhs) {
    switch(rhs) {
#define DEFINE_TO_STRING(Name) \
      case k##Name: return stream << #Name;
      FOR_EACH_TEXTURE2D_READER_STATE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return stream << "Unknown Texture2DReaderState";
    }
  }

  class Texture2DReaderHandler : public json::ReaderHandlerTemplate<Texture2DReaderState, Texture2DReaderHandler> {
    typedef json::ReaderHandlerTemplate<Texture2DReaderState, Texture2DReaderHandler> ParentType;
    static constexpr const auto kExpectedType = "Program";
  protected:
#define DEFINE_STATE_CHECK(Name)                                                          \
    inline bool Is##Name() const { return GetState() == Texture2DReaderState::k##Name; }
    FOR_EACH_TEXTURE2D_READER_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK

    bool OnParseDataField(const std::string& name) override;
  public:
    Texture2DReaderHandler():
      ParentType(kExpectedType) {
    }
    ~Texture2DReaderHandler() = default;
    bool String(const char* value, const rapidjson::SizeType length, const bool) override;
  };
}

#endif //PRT_TEXTURE_2D_JSON_H