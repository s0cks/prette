#include "prette/copy_to_image.h"

#include "prette/assertions.h"
#include "prette/copy_to_buffer.h"
#include "prette/image/image.h"
#include "prette/vk.h"
#include "prette/vk_buffer.h"
#include "prette/vk_cmd_buffers.h"

namespace prt::vk {
CopyBufferToImage::CopyBufferToImage(Buffer* source) :
  source_(source) {
  ASSERT_INITIALIZED(source_);
  region_ptr()->bufferOffset = 0;
  region_ptr()->bufferRowLength = 0;
  region_ptr()->bufferImageHeight = 0;
  region_ptr()->imageSubresource = {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .mipLevel = 0,
      .baseArrayLayer = 0,
      .layerCount = 1,
  };
  region_ptr()->imageOffset = {};
  region_ptr()->imageExtent = {};
}

void CopyBufferToImage::Apply(Image* dest, const VkImageLayout dst_layout) {
  ASSERT_INITIALIZED(dest);
  SingleUseCommandBuffer buffer{};
  vkCmdCopyBufferToImage(buffer, *GetSource(), *dest, dst_layout, 1, region_ptr());
}

void CopyBytesToImageWithStaging::Apply(Image* dest, const VkImageLayout dest_layout) {
  ASSERT_INITIALIZED(dest);
  BufferBuilder builder{};
  // clang-format off
  ScopedBuffer staging = builder.WithTransferSourceUsage()
    .WithSize(num_bytes_);
  // clang-format on
  ASSERT_INITIALIZED(staging);
  CopyBytesToBuffer::Copy(bytes_, num_bytes_, staging);
  CopyBufferToImage::Copy(staging, bounds_, dest, dest_layout);
}
}  // namespace prt::vk