#ifndef PRT_SHADER_H
#error "Please #include <prette/shader/shader.h> instead."
#endif //PRT_SHADER_H

#ifndef PRT_SHADER_TESS_CONTROL_H
#define PRT_SHADER_TESS_CONTROL_H

#include "prette/shader/shader.h"

namespace prt::shader {
  class TessControlShader : public ShaderTemplate<kTessControlShader> {
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