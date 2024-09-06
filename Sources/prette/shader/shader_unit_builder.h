#ifndef PRT_SHADER_UNIT_BUILDER_H
#define PRT_SHADER_UNIT_BUILDER_H

#include "prette/uri.h"
#include "prette/builder.h"
#include "prette/shader/shader_type.h"
#include "prette/shader/shader_code.h"

namespace prt::shader {
  class ShaderUnit;
  class ShaderUnitBuilder : public BuilderTemplate<ShaderUnit> {
    DEFINE_NON_COPYABLE_TYPE(ShaderUnitBuilder);
  private:
    ShaderType type_;
    ShaderCodeList code_;
  public:
    //TODO: pass-by-reference & move semantics for ShaderCodeList
    explicit ShaderUnitBuilder(const ShaderType type, const ShaderCodeList& code = {}):
      BuilderTemplate<ShaderUnit>(),
      type_(type),
      code_(code) {
    }
    ~ShaderUnitBuilder() override = default;

    auto GetType() const -> ShaderType {
      return type_;
    }

    auto GetCode() const -> const ShaderCodeList& {
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

    auto Build() const -> ShaderUnit* override;
    auto BuildAsync() const -> rx::observable<ShaderUnit*> override;
  };
}

#endif //PRT_SHADER_UNIT_BUILDER_H