#ifndef PRT_SHADER_TESS_EVAL_H
#define PRT_SHADER_TESS_EVAL_H

#include "prette/shader/shader.h"
#include "prette/shader/shader_unit.h"

namespace prt::shader {
  class TessEvalShader : public ShaderTemplate<kTessEvalShader> {
    DEFINE_NON_COPYABLE_TYPE(TessEvalShader);
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