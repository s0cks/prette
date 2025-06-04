#ifndef PRT_FRAMEBUFFER_H
#define PRT_FRAMEBUFFER_H

#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/framebuffer/framebuffer_attachment.h"
#include "prette/vk.h"

namespace prt {
class RenderPass;
class Framebuffer : public vk::HandleTemplate<VkFramebuffer> {
  friend class FramebufferBuilder;

 private:
  VkExtent2D extent_{};
  std::vector<FramebufferAttachment*> attachments_{};

  explicit Framebuffer(const VkFramebufferCreateInfo* create_info);

  auto Attach(FramebufferAttachment* rhs) -> FramebufferAttachment* {
    ASSERT(rhs);
    attachments_.push_back(rhs);
    return rhs;
  }

 public:
  ~Framebuffer() override;

  auto ToString() const -> std::string override;

  operator VkFramebuffer() const {
    return GetHandle();
  }
};
}  // namespace prt

#endif  // PRT_FRAMEBUFFER_H
