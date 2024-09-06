#ifndef PRT_SHADER_TESS_CONTROL_H
#define PRT_SHADER_TESS_CONTROL_H

#include "prette/shader/shader.h"
#include "prette/shader/shader_unit.h"

namespace prt::shader {
  class TessControlShader : public ShaderTemplate<kTessControlShader> {
    DEFINE_NON_COPYABLE_TYPE(TessControlShader);
  protected:
    explicit TessControlShader(const Metadata& meta, const ShaderId id):
      ShaderTemplate<kTessControlShader>(meta, id) {
    }
  public:
    ~TessControlShader() override = default;
    DECLARE_SHADER_TYPE(TessControl);
  };
}

#endif //PRT_SHADER_TESS_CONTROL_H