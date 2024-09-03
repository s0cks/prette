#ifndef PRT_SHADER_UNIT_BUILDER_H
#define PRT_SHADER_UNIT_BUILDER_H

#include "prette/uri.h"
#include "prette/builder.h"
#include "prette/shader/shader_type.h"
#include "prette/shader/shader_code.h"

namespace prt::shader {
  class ShaderUnit;
  class ShaderUnitBuilder : public BuilderTemplate<ShaderUnit> {
  protected:
    ShaderType type_;
    ShaderCodeList code_;
  public:
    explicit ShaderUnitBuilder(const ShaderType type, const ShaderCodeList& code = {}):
      BuilderTemplate<ShaderUnit>(),
      type_(type),
      code_(code) {
    }
    ~ShaderUnitBuilder() override = default;

    ShaderType GetType() const {
      return type_;
    }

    const ShaderCodeList& GetCode() const {
      return code_;
    }

    void Append(ShaderCode* code) {
      PRT_ASSERT(code);
      code_.push_back(code);
    }

    void Append(const uri::Uri& uri) {
      return Append(ShaderCode::FromFile(uri));
    }

    void Append(const ShaderCodeList& code) {
      PRT_ASSERT(!code.empty());
      std::for_each(std::begin(code), std::end(code), [this](ShaderCode* code) {
        code_.push_back(code);
      });
    }

    ShaderUnit* Build() const override;
    rx::observable<ShaderUnit*> BuildAsync() const override;
  };
}

#endif //PRT_SHADER_UNIT_BUILDER_H