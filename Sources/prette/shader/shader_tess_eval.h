#ifndef PRT_SHADER_H
#error "Please #include <prette/shader/shader.h> instead."
#endif //PRT_SHADER_H

#ifndef PRT_SHADER_TESS_EVAL_H
#define PRT_SHADER_TESS_EVAL_H

#include "prette/shader/shader.h"

namespace prt::shader {
  class TessEvalShader : public ShaderTemplate<kTessEvalShader> {
  protected:
    explicit TessEvalShader(const Metadata& meta, const ShaderId id):
      ShaderTemplate<kTessEvalShader>(meta, id) {  
    }
  public:
    ~TessEvalShader() override = default;
    DECLARE_SHADER_TYPE(TessEval);
  };
}

#endif //PRT_SHADER_TESS_EVAL_H