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
  protected:
    shader::ShaderType type_;
    uri::Uri uri_;

    ProgramShader(const shader::ShaderType type,
                  const uri::Uri& uri):
      type_(type),
      uri_(uri) {
    }
  public:
    ProgramShader() = default;
    ~ProgramShader() = default;

    shader::ShaderType GetType() const {
      return type_;
    }

    const uri::Uri& GetUri() const {
      return uri_;
    }

    Shader* GetShader() const;

    friend std::ostream& operator<<(std::ostream& stream, const ProgramShader& rhs) {
      stream << "ProgramShader(";
      stream << "type=" << rhs.GetType() << ", ";
      stream << "uri=" << rhs.GetUri();
      stream << ")";
      return stream;
    }
  };
  typedef std::vector<ProgramShader> ProgramShaderList;

  enum ProgramReaderState {
#define DEFINE_STATE(Name) k##Name,
    FOR_EACH_PROGRAM_READER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
  };
    
  static inline std::ostream&
  operator<<(std::ostream& stream, const ProgramReaderState& rhs) {
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
  protected:
    ProgramShaderList shaders_;

#define DEFINE_STATE_CHECK(Name)                                                          \
    inline bool Is##Name() const { return GetState() == ProgramReaderState::k##Name; }
    FOR_EACH_PROGRAM_READER_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK

    bool OnParseDataField(const std::string& name) override;
    bool OnParseProgramShaderRef(const shader::ShaderType type, const uri::Uri& uri);
  public:
    ProgramReaderHandler():
      json::ReaderHandlerTemplate<ProgramReaderState, ProgramReaderHandler>(kExpectedType),
      shaders_() {
      shaders_.reserve(5);
    }
    ~ProgramReaderHandler() = default;
    bool String(const char* value, const rapidjson::SizeType length, const bool) override;

    const ProgramShaderList& shaders() const {
      return shaders_;
    }

    ProgramShaderList::const_iterator shaders_begin() const {
      return shaders_.begin();
    }

    ProgramShaderList::const_iterator shaders_end() const {
      return shaders_.end();
    }
  };
}

#endif //PRT_PROGRAM_JSON_H