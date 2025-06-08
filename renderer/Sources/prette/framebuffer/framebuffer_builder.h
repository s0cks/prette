#ifndef PRT_FRAMEBUFFER_BUILDER_H
#define PRT_FRAMEBUFFER_BUILDER_H

#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/framebuffer/framebuffer.h"
#include "prette/glm.h"
#include "prette/image/image_view.h"
#include "prette/platform.h"
#include "prette/vk.h"

namespace prt {
class RenderPass;
class FramebufferBuilder : public vk::HandleBuilderTemplate<VkFramebufferCreateInfo, Framebuffer> {
  using ParentType = HandleBuilderTemplate<VkFramebufferCreateInfo, Framebuffer>;

 private:
  std::vector<VkImageView> attachments_{};

 public:
  FramebufferBuilder();
  ~FramebufferBuilder() override = default;

  auto WithRenderPass(const VkRenderPass rhs) -> FramebufferBuilder& {
    info_ptr()->renderPass = rhs;
    return *this;
  }

  auto WithRenderPass(vk::RenderPass* rhs) -> FramebufferBuilder&;

  auto WithFlags(const VkFramebufferCreateFlags rhs) -> FramebufferBuilder& {
    info_ptr()->flags = rhs;
    return *this;
  }

  auto WithWidth(const uint32_t rhs) -> FramebufferBuilder& {
    info_ptr()->width = rhs;
    return *this;
  }

  auto WithHeight(const uint32_t rhs) -> FramebufferBuilder& {
    info_ptr()->height = rhs;
    return *this;
  }

  auto WithSize(const uint32_t width, const uint32_t height) -> FramebufferBuilder& {
    info_ptr()->width = width;
    info_ptr()->height = height;
    return *this;
  }

  inline auto WithSize(const glm::u32vec2& size) -> FramebufferBuilder& {
    return WithSize(size[0], size[1]);
  }

  inline auto WithSize(const VkExtent2D& size) -> FramebufferBuilder& {
    return WithSize(size.width, size.height);
  }

  auto WithNext(const void* rhs) -> FramebufferBuilder& {
    info_ptr()->pNext = rhs;
    return *this;
  }

  auto WithLayers(const uint32_t rhs) -> FramebufferBuilder& {
    info_ptr()->layers = rhs;
    return *this;
  }

  auto ResetAttachments() -> FramebufferBuilder& {
    attachments_.clear();
    return *this;
  }

  auto WithAttachment(const VkImageView rhs) -> FramebufferBuilder& {
    ASSERT(rhs != VK_NULL_HANDLE);
    attachments_.push_back(rhs);
    return *this;
  }

  auto WithAttachment(vk::ImageView* rhs) -> FramebufferBuilder&;
  auto IsValid() const -> bool override;
  auto Build() -> Framebuffer* override;
  void BuildWithAttachments(const std::vector<vk::ImageView*>& attachments, std::vector<Framebuffer*>& results,
                            vk::ImageView* depth = nullptr);

  auto operator()() -> Framebuffer* {
    return Build();
  }

  operator Framebuffer*() {
    return Build();
  }
};
}  // namespace prt

#endif  // PRT_FRAMEBUFFER_BUILDER_H
