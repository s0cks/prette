#ifndef PRT_SWAPCHAIN_SYSTEM_H
#define PRT_SWAPCHAIN_SYSTEM_H

namespace prt {
class SwapchainSystem {
 public:
  static constexpr const auto kSystemName = "swapchain";

 public:
  SwapchainSystem();
  ~SwapchainSystem();
};
}  // namespace prt

#endif  // PRT_SWAPCHAIN_SYSTEM_H
