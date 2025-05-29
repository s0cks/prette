#include "prette/copy_to_buffer.h"

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/mapped_buffer_scope.h"
#include "prette/vk_buffer.h"
#include "prette/vk_cmd_buffers.h"

namespace prt::vk {
void CopyBytesToBuffer::operator()(Buffer* dst) const {
  ASSERT_INITIALIZED(dst);
  MappedBufferScope mapped(dst);
  ASSERT_MAPPED(mapped);
  memcpy(mapped, data(), size());
}

void CopyBytesToBufferWithStaging::operator()(Buffer* dst) const {
  ASSERT_INITIALIZED(dst);
  vk::BufferBuilder staging_builder{};
  staging_builder.WithTransferSourceUsage().WithSize(size());
  vk::ScopedBuffer staging(staging_builder);
  {
    vk::CopyBytesToBuffer copy(data(), size());
    copy(staging);
  }
  {
    vk::CopyBufferToBuffer copy(staging);
    copy(dst);
  }
}

CopyBufferToBuffer::CopyBufferToBuffer(Buffer* source, const uint64_t num_bytes, const uint64_t source_offset,
                                       const uint64_t dest_offset) :
  source_(source) {
  ASSERT_INITIALIZED(source_);
  copy_.size = num_bytes == VK_WHOLE_SIZE ? source->GetSize() : num_bytes;
  copy_.srcOffset = source_offset;
  copy_.dstOffset = dest_offset;
}

void CopyBufferToBuffer::operator()(Buffer* dst) const {
  ASSERT_INITIALIZED(dst);
  SingleUseCommandBuffer buffer;
  vkCmdCopyBuffer(buffer, *source_, *dst, 1, &copy_);
}
}  // namespace prt::vk