#ifndef PRT_STAGING_SCOPE_H
#define PRT_STAGING_SCOPE_H

#include <cstdint>

#include "prette/common.h"
#include "prette/vk.h"
#include "prette/vk_buffer.h"

namespace prt::vk {
class StagingScope {
 private:
  Buffer* buffer_ = nullptr;

 public:
  StagingScope(const uint8_t* data, const uint64_t num_bytes) {
    vk::BufferBuilder builder{};
    buffer_ = builder.BuildStagingBuffer(data, num_bytes);
    ASSERT_INITIALIZED(buffer_);
  }
  ~StagingScope() {
    delete buffer_;
  }

  auto GetBuffer() const -> Buffer* {
    return buffer_;
  }

  auto IsInitialized() const -> bool {
    return vk::IsInitialized(GetBuffer());
  }

  operator bool() const {
    return IsInitialized();
  }

  operator Buffer*() const {
    return buffer_;
  }
};
}  // namespace prt::vk

#endif  // PRT_STAGING_SCOPE_H
