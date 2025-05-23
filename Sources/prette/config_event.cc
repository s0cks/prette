#include "prette/config_event.h"

#include <string>

#include "prette/to_string.h"

namespace prt {
auto ConfigLoadEvent::ToString() const -> std::string {
  ToStringHelper<ConfigLoadEvent> helper{};
  return helper;
}

auto ConfigSaveEvent::ToString() const -> std::string {
  ToStringHelper<ConfigSaveEvent> helper{};
  return helper;
}
}  // namespace prt