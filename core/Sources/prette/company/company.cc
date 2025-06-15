#include "prette/company/company.h"

#include <string>

#include "prette/to_string.h"

namespace prt {
auto Company::ToString() const -> std::string {
  ToStringHelper<Company> helper{};
  helper.AddFieldRef("id", GetId());
  helper.AddField("name", GetName());
  return helper;
}
}  // namespace prt