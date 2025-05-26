#include "prette/swapchain/swapchain_event.h"

#include <string>

#include "prette/to_string.h"

namespace prt {
#define DEFINE_TOSTRING(Name)                       \
  auto Name##Event::ToString() const->std::string { \
    return ToStringHelper<Name##Event>();           \
  }
DEFINE_TOSTRING(SwapchainInit);
DEFINE_TOSTRING(SwapchainRecreated);
DEFINE_TOSTRING(SwapchainDeInit);

#undef DEFINE_TOSTRING

#define DEFINE_TOSTRING(Name)                       \
  auto Name##Event::ToString() const->std::string { \
    ToStringHelper<Name##Event> helper{};           \
    helper.AddFieldRef("reinit", IsReinit());       \
    return helper;                                  \
  }
DEFINE_TOSTRING(SwapchainCreated);
DEFINE_TOSTRING(SwapchainDestroyed);
#undef DEFINE_TOSTRING
}  // namespace prt