#include "prette/swapchain/swapchain_builder.h"

#include <array>

#include "prette/common.h"
#include "prette/gfx.h"
#include "prette/gfx_vk.h"
#include "prette/surface.h"
#include "prette/swapchain/swapchain.h"
#include "prette/vk.h"
#include "prette/vk_physical_device.h"

namespace prt {
SwapchainBuilder::SwapchainBuilder() :
  ParentType() {
  info_ptr()->sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
}

auto SwapchainBuilder::WithQueueFamilies(const QueueFamilyIndices& rhs) -> SwapchainBuilder& {
  queue_families_ = rhs;
  return *this;
}

auto SwapchainBuilder::WithSurface(vk::Surface* rhs) -> SwapchainBuilder& {
  ASSERT(rhs && rhs->GetSurface() != VK_NULL_HANDLE);
  const auto& surface_format = rhs->GetFormat();
  info_ptr()->surface = (*rhs);
  info_ptr()->minImageCount = rhs->GetImageCount();
  info_ptr()->imageFormat = surface_format.format;
  info_ptr()->imageColorSpace = surface_format.colorSpace;
  info_ptr()->imageExtent = rhs->GetExtent();
  const auto& surface_caps = rhs->GetCapabilities();
  if (surface_caps.supportedTransforms & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
    info_ptr()->imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  }
  if (surface_caps.supportedTransforms & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
    info_ptr()->imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  }
  info_ptr()->preTransform = surface_caps.currentTransform;
  info_ptr()->presentMode = rhs->GetPresentMode();
  const auto driver = Driver::Get();
  return WithQueueFamilies(driver->GetPhysicalDevice()->GetQueueFamilies());
}

auto SwapchainBuilder::IsValid() const -> bool {
  NOT_IMPLEMENTED(ERROR);  // TODO: implement
  return queue_families_.IsComplete();
}

auto SwapchainBuilder::Build() -> Swapchain* {
  ASSERT(IsValid());
  if (queue_families_.GetGraphicsFamily() != queue_families_.GetPresentFamily()) {
    std::array<uint32_t, 2> families = {
        queue_families_.GetGraphicsFamily(),
        queue_families_.GetPresentFamily(),
    };
    info_ptr()->imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    info_ptr()->queueFamilyIndexCount = families.size();
    info_ptr()->pQueueFamilyIndices = families.data();
  } else {
    info_ptr()->imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }
  return new Swapchain(&info());
}
}  // namespace prt