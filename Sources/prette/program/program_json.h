#ifndef PRT_PROGRAM_JSON_H
#define PRT_PROGRAM_JSON_H

#include <vector>
#include <ostream>
#include <glog/logging.h>

#include "prette/json.h"
#include "prette/shader/shader_type.h"

namespace prt {
  namespace shader {
    class Shader;
  }
  using shader::Shader;
}

namespace prt::program {
#define FOR_EACH_PROGRAM_READER_STATE(V)        \
  FOR_EACH_JSON_READER_TEMPLATE_STATE(V)        \
  V(FragmentShader)                             \
  V(VertexShader)                               \
  V(GeometryShader)                             \
  V(TessEvalShader)                             \
  V(TessControlShader)                          \
  V(IncludedShaders)

  class ProgramShader {
    friend class ProgramReaderHandler;
    friend class std::allocator<ProgramShader>;
    DEFINE_DEFAULT_COPYABLE_TYPE(ProgramShader);
  private:
    shader::ShaderType type_{};
    uri::Uri uri_{};
    
    ProgramShader(const shader::ShaderType type,
                  const uri::Uri& uri):
      type_(type),
      uri_(uri) {
    }
  public:
    ProgramShader() = default;
    ~ProgramShader() = default;

    auto GetType() const -> shader::ShaderType {
      return type_;
    }

    auto GetUri() const -> const uri::Uri& {
      return uri_;
    }

    auto GetShader() const -> Shader*;

    friend auto operator<<(std::ostream& stream, const ProgramShader& rhs) -> std::ostream& {
      stream << "ProgramShader(";
      stream << "type=" << rhs.GetType() << ", ";
      stream << "uri=" << rhs.GetUri();
      stream << ")";
      return stream;
    }
  };
  using ProgramShaderList = std::vector<ProgramShader>;

  enum ProgramReaderState {
#define DEFINE_STATE(Name) k##Name,
    FOR_EACH_PROGRAM_READER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
  };
    
  static inline auto
  operator<<(std::ostream& stream, const ProgramReaderState& rhs) -> std::ostream& {
    switch(rhs) {
#define DEFINE_TO_STRING(Name) \
      case k##Name: return stream << #Name;
      FOR_EACH_PROGRAM_READER_STATE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return stream << "Unknown ProgramReaderState";
    }
  }

  class ShaderCode;
  class ProgramReaderHandler : public json::ReaderHandlerTemplate<ProgramReaderState, ProgramReaderHandler> {
    static constexpr const auto kExpectedType = "Program";
    DEFINE_NON_COPYABLE_TYPE(ProgramReaderHandler);
  private:
    ProgramShaderList shaders_;

#define DEFINE_STATE_CHECK(Name)                                                          \
    inline auto Is##Name() const -> bool { return GetState() == ProgramReaderState::k##Name; }
    FOR_EACH_PROGRAM_READER_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK

    auto OnParseDataField(const std::string& name) -> bool override;
    auto OnParseProgramShaderRef(const shader::ShaderType type, const uri::Uri& uri) -> bool;
  public:
    ProgramReaderHandler():
      json::ReaderHandlerTemplate<ProgramReaderState, ProgramReaderHandler>(kExpectedType),
      shaders_() {
      shaders_.reserve(5);
    }
    ~ProgramReaderHandler() override = default;
    auto String(const char* value, const rapidjson::SizeType length, const bool) -> bool override;

    auto shaders() const -> const ProgramShaderList& {
      return shaders_;
    }

    auto shaders_begin() const -> ProgramShaderList::const_iterator {
      return shaders_.begin();
    }

    auto shaders_end() const -> ProgramShaderList::const_iterator {
      return shaders_.end();
    }
  };
}

#endif //PRT_PROGRAM_JSON_H