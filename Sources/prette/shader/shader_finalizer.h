#ifndef PRT_SHADER_FINALIZER_H
#define PRT_SHADER_FINALIZER_H

#include "prette/common.h"

namespace prt::shader {
  class ShaderFinalizer {
    DEFINE_NON_COPYABLE_TYPE(ShaderFinalizer);
  private:
    ShaderFinalizer() = default;
  public:
    ~ShaderFinalizer() = default;
  };
}

#endif //PRT_SHADER_FINALIZER_H