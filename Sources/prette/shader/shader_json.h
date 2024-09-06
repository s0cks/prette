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

  static inline auto
  operator<<(std::ostream& stream, const ShaderReaderState& rhs) -> std::ostream& {
    switch(rhs) {
#define DEFINE_TO_STRING(Name) \
      case k##Name: return stream << #Name;
      FOR_EACH_SHADER_READER_STATE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return stream << "Unknown ShaderReaderState";
    }
  }

  class ShaderReaderHandler : public json::ReaderHandlerTemplate<ShaderReaderState, ShaderReaderHandler> {
    DEFINE_NON_COPYABLE_TYPE(ShaderReaderHandler);
  private:
    static inline constexpr auto
    GetExpectedType(const ShaderType type) -> const char* {
      switch(type) {
        case kVertexShader:
          return "VertexShader";
        case kFragmentShader:
          return "FragmentShader";
        default:
          return "Shader";
      }
    }
  private:
    ShaderType type_;
    std::vector<std::string> sources_;

#define DEFINE_STATE_CHECK(Name)                                                                \
    inline auto Is##Name() const -> bool { return GetState() == ShaderReaderState::k##Name; }
    FOR_EACH_SHADER_READER_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK

    auto OnParseDataField(const std::string& name) -> bool override;
    auto OnParseSource(const std::string& value) -> bool;
  public:
    explicit ShaderReaderHandler(const ShaderType type):
      json::ReaderHandlerTemplate<ShaderReaderState, ShaderReaderHandler>(GetExpectedType(type)),
      type_(type),
      sources_() {
    }
    ~ShaderReaderHandler() override = default;

    auto GetShaderTye() const -> ShaderType {
      return type_;
    }

    auto GetSources() const -> const std::vector<std::string>& {
      return sources_;
    }

    auto StartArray() -> bool override;
    auto EndArray(const json::SizeType size) -> bool override;
    auto String(const char* value, const rapidjson::SizeType length, const bool) -> bool override;
  };
}

#endif //PRT_SHADER_JSON_H