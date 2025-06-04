#include "prette/framebuffer/framebuffer.h"

#include "prette/gfx_vk.h"
#include "prette/to_string.h"
#include "prette/vk.h"

namespace prt {
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
}  // namespace prt