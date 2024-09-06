#ifndef PRT_SHADER_VERTEX_H
#define PRT_SHADER_VERTEX_H

#include "prette/shader/shader.h"
#include "prette/shader/shader_unit.h"

namespace prt::shader {
  class VertexShader : public ShaderTemplate<kVertexShader> {
    DEFINE_NON_COPYABLE_TYPE(VertexShader);
  protected:
    explicit VertexShader(const Metadata& meta, const ShaderId id):
      ShaderTemplate<kVertexShader>(meta, id) {  
    }
  public:
    ~VertexShader() override = default;
    DECLARE_SHADER_TYPE(Vertex);
  public:
    static auto New(const uri::Uri& uri) -> VertexShader*;
    static auto FromSource(const uri::Uri& uri) -> VertexShader*;
    static auto FromJson(const uri::Uri& uri) -> VertexShader*;
    static auto FromJson(const char* source, const uword length) -> VertexShader*;

    static inline auto
    FromJson(const char* source) -> VertexShader* {
      return FromJson(source, strlen(source));
    }

    static inline auto
    FromJson(const std::string& source) -> VertexShader* {
      return FromJson(source.data(), source.length());
    }
  };
}

#endif //PRT_SHADER_VERTEX_H