#ifndef PRT_SHADER_FRAGMENT_H
#define PRT_SHADER_FRAGMENT_H

#include "prette/shader/shader.h"
#include "prette/shader/shader_unit.h"

namespace prt::shader {
  class FragmentShader : public ShaderTemplate<kFragmentShader> {
    DEFINE_NON_COPYABLE_TYPE(FragmentShader);
  protected:
    explicit FragmentShader(const Metadata& meta, const ShaderId id):
      ShaderTemplate<kFragmentShader>(meta, id) {  
    }
  public:
    ~FragmentShader() override = default;
    DECLARE_SHADER_TYPE(Fragment);
  public:
    static auto New(const uri::Uri& uri) -> FragmentShader*;
    static auto FromSource(const uri::Uri& uri) -> FragmentShader*;
    static auto FromJson(const uri::Uri& uri) -> FragmentShader*;
    static auto FromJson(const std::string& value) -> FragmentShader*;
  };
}

#endif //PRT_SHADER_FRAGMENT_H