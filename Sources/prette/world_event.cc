#include "prette/world_event.h"

#include "prette/to_string.h"

namespace prt {
auto WorldInitEvent::ToString() const -> std::string {
  return ToStringHelper<WorldInitEvent>{};
}

auto WorldDeInitEvent::ToString() const -> std::string {
  return ToStringHelper<WorldDeInitEvent>{};
}
}  // namespace prt