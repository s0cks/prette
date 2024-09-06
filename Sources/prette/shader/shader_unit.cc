#include "prette/shader/shader_unit.h"
#include <sstream>
#include "prette/merkle.h"
#include "prette/shader/shader_code.h"

namespace prt::shader {
  using merkle::Tree;

  ShaderUnit::ShaderUnit(const Metadata& meta,
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
    Tree tree(hashes);
    hash_ = tree.GetRootHash();
  }

  auto ShaderUnit::Accept(ShaderCodeVisitor* vis) -> bool {
    PRT_ASSERT(vis);
    for(const auto& code : code_) {
      if(!vis->VisitShaderCode(code))
        return false;
    }
    return true;
  }

  auto ShaderUnit::ToString() const -> std::string {
    std::stringstream ss;
    ss << "ShaderUnit(";
    ss << "meta=" << GetMeta() << ", ";
    ss << "type=" << GetType() << ", ";
    if(!IsEmpty())
      ss << "hash=" << GetHash();
    ss << ")";
    return ss.str();
  }
}