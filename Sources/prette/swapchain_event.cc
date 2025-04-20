#include "prette/swapchain_event.h"

#include "prette/to_string.h"

namespace prt {
auto SwapChainInitEvent::ToString() const -> std::string {
  return ToStringHelper<SwapChainInitEvent>{};
}

auto SwapChainDestroyedEvent::ToString() const -> std::string {
  return ToStringHelper<SwapChainDestroyedEvent>{};
}
}  // namespace prt