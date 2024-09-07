#ifndef PRT_SHADER_SOURCE_ATTACHER_H
#define PRT_SHADER_SOURCE_ATTACHER_H

#include "prette/shader/shader_id.h"
#include "prette/shader/shader_code.h"

namespace prt::shader {
  class ShaderUnit;
  class ShaderSourceAttacher : protected ShaderCodeVisitor {
    DEFINE_NON_COPYABLE_TYPE(ShaderSourceAttacher);
  private:
    uword num_attached_;
    std::vector<GLchar*> sources_;
    std::vector<GLint> lengths_;
  public:
    explicit ShaderSourceAttacher(ShaderUnit* unit = nullptr);
    ~ShaderSourceAttacher() override = default;

    auto GetNumberOfSourcesAttached() const -> uword {
      return num_attached_;
    }

    auto GetSourceAt(const uword idx) const -> std::string {
      PRT_ASSERT(idx >= 0 && idx <= GetNumberOfSourcesAttached());
      return {sources_[idx], static_cast<size_t>(lengths_[idx]) };
    }

    void Append(ShaderUnit* unit);
    void Append(ShaderCode* code);

    auto VisitShaderCode(ShaderCode* code) -> bool override {
      Append(code);
      return true;
    }

    auto AttachSources(const ShaderId target) -> bool;
  };
}

#endif // PRT_SHADER_SOURCE_ATTACHER_H