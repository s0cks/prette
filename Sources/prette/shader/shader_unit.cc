#include "prette/shader/shader_unit.h"
#include <sstream>
#include "prette/merkle.h"
#include "prette/shader/shader_code.h"

namespace prt::shader {
  std::string ShaderUnit::ToString() const {
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