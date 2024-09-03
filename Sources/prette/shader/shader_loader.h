#ifndef PRT_SHADER_LOADER_H
#define PRT_SHADER_LOADER_H

namespace prt::shader {
  class Shader;
  class ShaderLoader {
  protected:
    ShaderLoader() = default;
  public:
    virtual ~ShaderLoader() = default;
    virtual Shader* LoadShader() const = 0;
  };
}

#endif //PRT_SHADER_LOADER_H