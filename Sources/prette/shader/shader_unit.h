#ifndef PRT_SHADER_UNIT_H
#define PRT_SHADER_UNIT_H

#include <vector>
#include "prette/common.h"
#include "prette/object.h"
#include "prette/uint256.h"
#include "prette/shader/shader_type.h"
#include "prette/shader/shader_code.h"

namespace prt::shader {
  class ShaderUnit : public Object {
    DEFINE_NON_COPYABLE_TYPE(ShaderUnit);
  private:
    ShaderType type_;
    ShaderCodeList code_;
    uint256 hash_;

    explicit ShaderUnit(const Metadata& meta,
                        const ShaderType type,
                        const ShaderCodeList& code);
  public:
    ~ShaderUnit() override = default;
    auto ToString() const -> std::string override;

    auto GetType() const -> ShaderType {
      return type_;
    }

    auto GetSize() const -> uword {
      return code_.size();
    }

    auto IsEmpty() const -> bool {
      return code_.empty();
    }

    auto GetSource(const uword idx) const -> ShaderCode* {
      PRT_ASSERT(idx >= 0 && idx <= GetSize());
      return code_[idx];
    }

    auto begin() const -> ShaderCodeList::const_iterator {
      return std::begin(code_);
    }

    auto end() const -> ShaderCodeList::const_iterator {
      return std::end(code_);
    }

    auto GetCode() const -> const ShaderCodeList& {
      return code_;
    }

    auto GetHash() const -> const uint256& {
      return hash_;
    }

    auto Accept(ShaderCodeVisitor* vis) -> bool;
  public:
    static inline auto
    New(const Metadata& meta, const ShaderType type, const ShaderCodeList& code = {}) -> ShaderUnit* {
      return new ShaderUnit(meta, type, code);
    }

    static inline auto
    New(const ShaderType type, const ShaderCodeList& code = {}) -> ShaderUnit* {
      Metadata meta;
      return New(meta, type, code);
    }

#define DEFINE_NEW_SHADER_UNIT_BY_TYPE(Name, Ext, GlValue)                                                \
    static inline auto                                                                                    \
    New##Name##ShaderUnit(const Metadata& meta = {}, const ShaderCodeList& code = {}) -> ShaderUnit* {    \
      return New(meta, k##Name##Shader, code);                                                            \
    }

    FOR_EACH_SHADER_TYPE(DEFINE_NEW_SHADER_UNIT_BY_TYPE)
#undef DEFINE_NEW_SHADER_UNIT_BY_TYPE
  };

  static inline auto
  operator<<(std::ostream& stream, ShaderUnit* rhs) -> std::ostream& {
    return stream << (rhs ? rhs->ToString() : "null");
  }
}

#endif //PRT_SHADER_UNIT_H