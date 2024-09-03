#ifndef PRT_SHADER_H
#error "Please #include <prette/shader/shader.h> instead."
#endif //PRT_SHADER_H

#ifndef PRT_SHADER_FRAGMENT_H
#define PRT_SHADER_FRAGMENT_H

#include "prette/shader/shader.h"

namespace prt::shader {
  class FragmentShader : public ShaderTemplate<kFragmentShader> {
  protected:
    explicit FragmentShader(const Metadata& meta, const ShaderId id):
      ShaderTemplate<kFragmentShader>(meta, id) {  
    }
  public:
    ~FragmentShader() override = default;
    DECLARE_SHADER_TYPE(Fragment);
  public:
    static FragmentShader* New(const uri::Uri& uri);
    static FragmentShader* FromSource(const uri::Uri& uri);
    static FragmentShader* FromJson(const uri::Uri& uri);
    static FragmentShader* FromJson(const std::string& value);
  };
}

#endif //PRT_SHADER_FRAGMENT_H