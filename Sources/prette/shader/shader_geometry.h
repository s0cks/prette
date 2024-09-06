#ifndef PRT_SHADER_GEOMETRY_H
#define PRT_SHADER_GEOMETRY_H

#include "prette/shader/shader.h"
#include "prette/shader/shader_unit.h"

namespace prt::shader {
  class GeometryShader : public ShaderTemplate<kGeometryShader> {
    DEFINE_NON_COPYABLE_TYPE(GeometryShader);
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