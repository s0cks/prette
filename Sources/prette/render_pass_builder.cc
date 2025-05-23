#include "prette/render_pass_builder.h"

#include <vector>

#include "prette/render_pass.h"
#include "prette/vk.h"

namespace prt::vk {
RenderPassBuilder::RenderPassBuilder() {
  attachments_.reserve(2);
  subpass_deps_.reserve(2);
  subpasses_.reserve(2);
  info_ptr()->sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
}

RenderPassBuilder::~RenderPassBuilder() {}

auto RenderPassBuilder::IsValid() const -> bool {
  return true;
}

auto RenderPassBuilder::Build() -> RenderPass* {
  UpdateInfo();
  return new RenderPass(name_, info_ptr());
}
}  // namespace prt::vk