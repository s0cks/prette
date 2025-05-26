#include "prette/copy_to_image.h"

#include "prette/image/image.h"
#include "prette/vk.h"
#include "prette/vk_buffer.h"
#include "prette/vk_cmd_buffers.h"

namespace prt::vk {
void CopyBufferToImage::Apply(Buffer* src, Image* dst, const VkImageLayout dst_layout) {
  SingleUseCommandBuffer buffer{};
  vkCmdCopyBufferToImage(buffer, *src, *dst, dst_layout, 1, region_ptr());
}
}  // namespace prt::vk