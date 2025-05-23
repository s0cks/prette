#include "prette/framebuffer.h"

#include <string>
#include <vector>

#include "prette/common.h"
#include "prette/gfx_vk.h"
#include "prette/image_view.h"
#include "prette/render_pass.h"
#include "prette/to_string.h"
#include "prette/vk.h"

namespace prt::vk {
FramebufferBuilder::FramebufferBuilder() :
  ParentType() {
  info_ptr()->sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  info_ptr()->attachmentCount = 0;
  info_ptr()->pAttachments = nullptr;
  info_ptr()->renderPass = VK_NULL_HANDLE;
  info_ptr()->flags = 0;
  info_ptr()->height = 0;
  info_ptr()->width = 0;
  info_ptr()->pNext = nullptr;
  info_ptr()->layers = 0;
}

auto FramebufferBuilder::WithAttachment(ImageView* rhs) -> FramebufferBuilder& {
  ASSERT_INITIALIZED(rhs);
  return WithAttachment(*rhs);
}

auto FramebufferBuilder::WithRenderPass(RenderPass* rhs) -> FramebufferBuilder& {
  ASSERT(rhs);
  return WithRenderPass(*rhs);
}

auto FramebufferBuilder::IsValid() const -> bool {
  return true;
}

auto FramebufferBuilder::Build() -> Framebuffer* {
  ASSERT(IsValid());
  info_ptr()->attachmentCount = attachments_.size();
  info_ptr()->pAttachments = attachments_.data();
  return new Framebuffer(&info());
}

void FramebufferBuilder::BuildWithAttachments(const std::vector<vk::ImageView*>& attachments,
                                              std::vector<vk::Framebuffer*>& results) {
  results.resize(attachments.size());
  for (auto idx = 0; idx < attachments.size(); idx++) {
    // clang-format off
    results[idx] = ResetAttachments()
      .WithAttachment(attachments[idx])
      .Build();
    // clang-format on
    ASSERT_INITIALIZED(results[idx]);
  }
}

Framebuffer::Framebuffer(const VkFramebufferCreateInfo* create_info) :
  HandleTemplate<VkFramebuffer>() {
  const auto driver = Driver::Get();
  driver->CreateFramebuffer(create_info, handle_ptr());
}

Framebuffer::~Framebuffer() {
  const auto driver = Driver::Get();
  driver->DestroyFramebuffer(handle_ref());
}

auto Framebuffer::ToString() const -> std::string {
  return ToStringHelper<Framebuffer>{};
}
}  // namespace prt::vk