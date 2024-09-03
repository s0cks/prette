#ifndef PRT_SHADER_H
#error "Please #include <prette/shader/shader.h> instead."
#endif //PRT_SHADER_H

#ifndef PRT_SHADER_GEOMETRY_H
#define PRT_SHADER_GEOMETRY_H

#include "prette/shader/shader.h"

namespace prt::shader {
  class GeometryShader : public ShaderTemplate<kGeometryShader> {
  protected:
    explicit GeometryShader(const Metadata& meta, const ShaderId id):
      ShaderTemplate<kGeometryShader>(meta, id) {
    }
  public:
    ~GeometryShader() override = default;
    DECLARE_SHADER_TYPE(Geometry);
  };
}

#endif //PRT_SHADER_GEOMETRY_H