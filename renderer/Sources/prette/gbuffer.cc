#include "prette/gbuffer.h"

#include <utility>
#include <vulkan/vulkan_core.h>

namespace prt {
GBuffer::GBuffer(const VkExtent2D extent) :
  extent_(std::move(extent)) {}

GBuffer::~GBuffer() {
  delete framebuffer_;
  for (auto& attachment : color_attachments_)
    delete attachment;
  delete depth_attachment_;
}
}  // namespace prt
