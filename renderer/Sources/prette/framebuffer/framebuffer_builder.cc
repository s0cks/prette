#include "prette/framebuffer/framebuffer_builder.h"

#include <vector>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/framebuffer/framebuffer.h"
#include "prette/image/image_view.h"
#include "prette/render_pass/render_pass.h"
#include "prette/vk.h"

namespace prt {
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

auto FramebufferBuilder::WithAttachment(vk::ImageView* rhs) -> FramebufferBuilder& {
  ASSERT_INITIALIZED(rhs);
  return WithAttachment(*rhs);
}

auto FramebufferBuilder::WithRenderPass(vk::RenderPass* rhs) -> FramebufferBuilder& {
  ASSERT(rhs);
  return WithRenderPass(*rhs);
}

auto FramebufferBuilder::IsValid() const -> bool {
  NOT_IMPLEMENTED(WARNING);  // TODO: implement @s0cks
  return true;
}

auto FramebufferBuilder::Build() -> Framebuffer* {
  ASSERT(IsValid());
  info_ptr()->attachmentCount = attachments_.size();
  info_ptr()->pAttachments = attachments_.data();
  return new Framebuffer(&info());
}

void FramebufferBuilder::BuildWithAttachments(const std::vector<vk::ImageView*>& attachments,
                                              std::vector<Framebuffer*>& results, vk::ImageView* depth) {
  results.resize(attachments.size());
  for (auto idx = 0; idx < attachments.size(); idx++) {
    ResetAttachments().WithAttachment(attachments[idx]);
    if (depth)
      WithAttachment(depth);
    results[idx] = Build();
    ASSERT_INITIALIZED(results[idx]);
  }
}
}  // namespace prt