#ifndef PRT_SHADER_JSON_H
#define PRT_SHADER_JSON_H

#include "prette/json.h"
#include "prette/shader/shader_type.h"

namespace prt::shader {
#define FOR_EACH_SHADER_READER_STATE(V)         \
  FOR_EACH_JSON_READER_TEMPLATE_STATE(V)        \
  V(Sources)

  enum ShaderReaderState {
#define DEFINE_STATE(Name) k##Name,
    FOR_EACH_SHADER_READER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
  };

  static inline std::ostream&
  operator<<(std::ostream& stream, const ShaderReaderState& rhs) {
    switch(rhs) {
#define DEFINE_TO_STRING(Name) \
      case k##Name: return stream << #Name;
      FOR_EACH_SHADER_READER_STATE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return stream << "Unknown ShaderReaderState";
    }
  }

  class ShaderReaderHandler : public json::ReaderHandlerTemplate<ShaderReaderState, ShaderReaderHandler> {
    static inline constexpr const char*
    GetExpectedType(const ShaderType type) {
      switch(type) {
        case kVertexShader:
          return "VertexShader";
        case kFragmentShader:
          return "FragmentShader";
        default:
          return "Shader";
      }
    }
  protected:
    ShaderType type_;
    std::vector<std::string> sources_;

#define DEFINE_STATE_CHECK(Name)                                                        \
    inline bool Is##Name() const { return GetState() == ShaderReaderState::k##Name; }
    FOR_EACH_SHADER_READER_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK

    bool OnParseDataField(const std::string& name) override;
    bool OnParseSource(const std::string& value);
  public:
    explicit ShaderReaderHandler(const ShaderType type):
      json::ReaderHandlerTemplate<ShaderReaderState, ShaderReaderHandler>(GetExpectedType(type)),
      type_(type),
      sources_() {
    }
    ~ShaderReaderHandler() override = default;

    bool StartArray() override;
    bool EndArray(const json::SizeType size) override;

    bool String(const char* value, const rapidjson::SizeType length, const bool) override;

    ShaderType GetShaderTye() const {
      return type_;
    }

    const std::vector<std::string>& GetSources() const {
      return sources_;
    }
  };
}

#endif //PRT_SHADER_JSON_H