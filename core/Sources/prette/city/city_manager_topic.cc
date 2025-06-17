#include "prette/city/city_manager_topic.h"

#include <string>

#include "prette/to_string.h"

namespace prt {
auto CityChangedEvent::ToString() const -> std::string {
  ToStringHelper<CityChangedEvent> helper{};
  helper.AddFieldPtr("previous", GetPreviousCity());
  helper.AddFieldPtr("current", GetCurrentCity());
  return helper;
}
}  // namespace prt