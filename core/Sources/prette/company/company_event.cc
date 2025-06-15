#include "prette/company/company_event.h"

#include "prette/to_string.h"

namespace prt {
#define DEFINE_TOSTRING(Name)                       \
  auto Name##Event::ToString() const->std::string { \
    ToStringHelper<Name##Event> helper{};           \
    helper.AddFieldPtr("company", GetCompany());    \
    return helper;                                  \
  }
FOR_EACH_COMPANY_EVENT(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
}  // namespace prt