#include "prette/surface.h"

#include <string>

#include "prette/assertions.h"
#include "prette/gfx.h"
#include "prette/gfx_driver.h"
#include "prette/gfx_driver_event.h"
#include "prette/swapchain/swapchain_support.h"
#include "prette/to_string.h"
#include "prette/vk.h"
#include "prette/vk_instance.h"  // IWYU pragma: keep
#include "prette/vk_physical_device.h"
#include "prette/window/window.h"

namespace prt::vk {
static inline auto IsValidSwapchainSurfaceFormat(const VkSurfaceFormatKHR& format) -> bool {
  return format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
}

static inline auto IsValidSwapchainPresentMode(const VkPresentModeKHR& mode) -> bool {
  return mode == VK_PRESENT_MODE_MAILBOX_KHR;
}

Surface::Surface(Driver* driver) {
  const auto window = GetAppWindow();
  ASSERT(window);
  CHECK_VK(FATAL,
           glfwCreateWindowSurface(*driver->GetInstance(), window->GetHandle(), driver->GetAllocator(), handle_ptr()),
           "failed to create window VkSurfaceKHR");
  OnPhysicalDeviceInitEvent().subscribe([this, driver](PhysicalDeviceInitEvent* event) {
    const auto support = QuerySwapchainSupport(*driver->GetPhysicalDevice(), handle_ref());
    format_ = support.FindSurfaceFormat(&IsValidSwapchainSurfaceFormat);
    present_mode_ = support.FindPresentMode(&IsValidSwapchainPresentMode);
    extent_ = support.GetExtent();
    capabilities_ = support.surface_capabilities;
    image_count_ = capabilities_.minImageCount + 1;
    support.ClampImageCount(image_count_);
  });
}

Surface::~Surface() {
  const auto driver = Driver::Get();
  ASSERT(driver);
  vkDestroySurfaceKHR(*driver->GetInstance(), handle_ref(), driver->GetAllocator());
}

auto Surface::ToString() const -> std::string {
  return ToStringHelper<Surface>{};
}
}  // namespace prt::vk