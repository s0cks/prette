#include "prette/swapchain_target.h"

#include "prette/to_string.h"

namespace prt {
SwapChainTarget::SwapChainTarget(const uint64_t idx, const VkFormat& format, const VkRenderPass& pass, const VkExtent2D& extent,
                                 const VkImage& image) :
  index_(idx) {
  const auto driver = Driver::Get();
  {
    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = image;
    create_info.format = format;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;
    CHECK_VK(FATAL, vkCreateImageView(driver->GetDevice(), &create_info, driver->GetAllocator(), &view_),
             "failed to create vk image view");
  }
  {
    std::array<VkImageView, 1> attachments = {
        view_,
    };
    VkFramebufferCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    create_info.renderPass = pass;
    create_info.attachmentCount = attachments.size();
    create_info.pAttachments = attachments.data();
    create_info.width = extent.width;
    create_info.height = extent.height;
    create_info.layers = 1;
    CHECK_VK(FATAL, vkCreateFramebuffer(driver->GetDevice(), &create_info, driver->GetAllocator(), &framebuffer_),
             "failed to create vk framebuffer");
  }
}

SwapChainTarget::~SwapChainTarget() {
  const auto driver = Driver::Get();
  driver->Destroy(view_, vkDestroyImageView);
  driver->Destroy(framebuffer_, vkDestroyFramebuffer);
}

auto SwapChainTarget::ToString() const -> std::string {
  ToStringHelper<SwapChainTarget> helper{};
  helper.AddFieldRef("index", GetIndex());
  return helper;
}
}  // namespace prt