#include "prette/person.h"

#include "prette/to_string.h"

namespace prt {
auto Person::ToString() const -> std::string {
  ToStringHelper<Person> helper{};
  helper.AddField("first_name", GetFirstName());
  helper.AddField("last_name", GetLastName());
  helper.AddFieldRef("gender", GetGender());
  return helper;
}
}  // namespace prt