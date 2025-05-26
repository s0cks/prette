#ifndef PRT_SWAPCHAIN_STATE_H
#define PRT_SWAPCHAIN_STATE_H

#include <cstdint>
#include <ostream>

namespace prt {
#define FOR_EACH_SWAPCHAIN_STATE(V) \
  V(SwapchainInit)                  \
  V(SwapchainCreated)               \
  V(SwapchainDestroyed)             \
  V(SwapchainRecreated)             \
  V(SwapchainDeInit)

enum SwapchainState {
#define DEFINE_STATE(Name) k##Name,
  FOR_EACH_SWAPCHAIN_STATE(DEFINE_STATE)
#undef DEFINE_STATE
};

static inline auto operator<<(std::ostream& stream, const SwapchainState& rhs) -> std::ostream& {
  switch (rhs) {
#define DEFINE_TOSTRING(Name)   \
  case SwapchainState::k##Name: \
    stream << #Name;
    FOR_EACH_SWAPCHAIN_STATE(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
    default:
      return stream << "undefined SwapchainState: " << static_cast<int64_t>(rhs);
  }
}
}  // namespace prt

#endif  // PRT_SWAPCHAIN_STATE_H
