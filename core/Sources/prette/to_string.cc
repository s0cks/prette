#include "prette/to_string.h"

#include <sstream>
#include <string>

namespace prt::tostring {
auto ToStringHelperBase::ToString() const -> std::string {
  std::stringstream ss{};
  ss << GetTypename() << "{";
  auto remaining = fields_.size();
  for (const auto& field : fields_) {
    ss << field;
    if (--remaining > 0)
      ss << ", ";
  }
  ss << "}";
  return ss.str();
}
}  // namespace prt::tostring