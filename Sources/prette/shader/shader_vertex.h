#ifndef PRT_SHADER_H
#error "Please #include <prette/shader/shader.h> instead."
#endif //PRT_SHADER_H

#ifndef PRT_SHADER_VERTEX_H
#define PRT_SHADER_VERTEX_H

#include "prette/shader/shader.h"

namespace prt::shader {
  class VertexShader : public ShaderTemplate<kVertexShader> {
  protected:
    explicit VertexShader(const Metadata& meta, const ShaderId id):
      ShaderTemplate<kVertexShader>(meta, id) {  
    }
  public:
    ~VertexShader() override = default;
    DECLARE_SHADER_TYPE(Vertex);
  public:
    static VertexShader* New(const uri::Uri& uri);
    static VertexShader* FromSource(const uri::Uri& uri);
    static VertexShader* FromJson(const uri::Uri& uri);
    static VertexShader* FromJson(const char* source, const uword length);

    static inline VertexShader*
    FromJson(const char* source) {
      return FromJson(source, strlen(source));
    }

    static inline VertexShader*
    FromJson(const std::string& source) {
      return FromJson(source.data(), source.length());
    }
  };
}

#endif //PRT_SHADER_VERTEX_H