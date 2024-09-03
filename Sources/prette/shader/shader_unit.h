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
  protected:
    ShaderType type_;
    ShaderCodeList code_;
    uint256 hash_;

    explicit ShaderUnit(const Metadata& meta,
                        const ShaderType type,
                        const ShaderCodeList& code):
      Object(meta),
      type_(type),
      code_(code),
      hash_() {
      std::vector<uint256> hashes;
      hashes.reserve(code.size());
      std::for_each(std::begin(code), std::end(code), [&hashes](ShaderCode* code) {
        hashes.push_back(code->GetSHA256());
      });
      merkle::Tree tree(hashes);
      hash_ = tree.GetRootHash();
    }
  public:
    ~ShaderUnit() override = default;
    std::string ToString() const override;

    ShaderType GetType() const {
      return type_;
    }

    uword GetSize() const {
      return code_.size();
    }

    bool IsEmpty() const {
      return code_.empty();
    }

    ShaderCode* GetSource(const uword idx) const {
      PRT_ASSERT(idx >= 0 && idx <= GetSize());
      return code_[idx];
    }

    ShaderCodeList::const_iterator begin() const {
      return std::begin(code_);
    }

    ShaderCodeList::const_iterator end() const {
      return std::end(code_);
    }

    const ShaderCodeList& GetCode() const {
      return code_;
    }

    const uint256& GetHash() const {
      return hash_;
    }

    bool Accept(ShaderCodeVisitor* vis) {
      PRT_ASSERT(vis);
      for(const auto& code : code_) {
        if(!vis->VisitShaderCode(code))
          return false;
      }
      return true;
    }
  public:
    static inline ShaderUnit*
    New(const Metadata& meta, const ShaderType type, const ShaderCodeList& code = {}) {
      return new ShaderUnit(meta, type, code);
    }

    static inline ShaderUnit*
    New(const ShaderType type, const ShaderCodeList& code = {}) {
      Metadata meta;
      return New(meta, type, code);
    }

#define DEFINE_NEW_SHADER_UNIT_BY_TYPE(Name, Ext, GlValue)                                 \
    static inline ShaderUnit*                                                              \
    New##Name##ShaderUnit(const Metadata& meta = {}, const ShaderCodeList& code = {}) {    \
      return New(meta, k##Name##Shader, code);                                             \
    }

    FOR_EACH_SHADER_TYPE(DEFINE_NEW_SHADER_UNIT_BY_TYPE)
#undef DEFINE_NEW_SHADER_UNIT_BY_TYPE
  };
}

#endif //PRT_SHADER_UNIT_H