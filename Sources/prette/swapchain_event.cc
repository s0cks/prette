#include "prette/swapchain_event.h"

#include <string>

#include "prette/to_string.h"

namespace prt {
auto SwapchainCreatedEvent::ToString() const -> std::string {
  return ToStringHelper<SwapchainCreatedEvent>();
}

auto SwapchainInitEvent::ToString() const -> std::string {
  return ToStringHelper<SwapchainInitEvent>{};
}

auto SwapchainDestroyedEvent::ToString() const -> std::string {
  return ToStringHelper<SwapchainDestroyedEvent>{};
}
}  // namespace prt