#ifndef PRT_VAO_JSON_H
#define PRT_VAO_JSON_H

#include <cstdint>
#include <ostream>
#include "prette/json.h"
#include "prette/metadata.h"

namespace prt::vao {
#define FOR_EACH_VAO_READER_STATE(V)        \
  V(Error)                                  \
  V(Open)                                   \
  V(Type)                                   \
  V(Meta)                                   \
  V(MetaName)                               \
  V(MetaTags)                               \
  V(Data)                                   \
  V(Closed)

  enum VaoReaderState : uint16_t {
#define DEFINE_STATE(Name) k##Name,
    FOR_EACH_VAO_READER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
  };

  static inline std::ostream&
  operator<<(std::ostream& stream, const VaoReaderState& rhs) {
    switch(rhs) {
#define DEFINE_TO_STRING(Name) \
      case k##Name: return stream << #Name;
      FOR_EACH_VAO_READER_STATE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return stream << "Unknown VaoReaderHandler::State: " << static_cast<uint16_t>(rhs);
    }
  }

  class VaoReaderHandler : public json::ReaderHandlerTemplate<VaoReaderState, VaoReaderHandler> {
    static constexpr const auto kExpectedType = "Vao";
  protected:
    Metadata meta_;

#define DEFINE_STATE_CHECK(Name)                                            \
    inline bool Is##Name() const { return GetState() == VaoReaderState::k##Name; }
    FOR_EACH_VAO_READER_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK

    bool OnParseDataField(const std::string& name) override;
  public:
    VaoReaderHandler():
      json::ReaderHandlerTemplate<VaoReaderState, VaoReaderHandler>(kExpectedType) {
    }
    virtual ~VaoReaderHandler() = default;

    const Metadata& GetMeta() const {
      return meta_;
    }
  };
}

#endif //PRT_VAO_JSON_H