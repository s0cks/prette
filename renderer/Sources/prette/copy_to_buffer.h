#ifndef PRT_COPY_TO_BUFFER_H
#define PRT_COPY_TO_BUFFER_H

#include <cstdint>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/std140.h"
#include "prette/vk.h"

namespace prt::vk {
class CopyBytesToBufferTemplate {
  DEFINE_NON_COPYABLE_TYPE(CopyBytesToBufferTemplate);

 private:
  const uint8_t* source_;
  uint64_t source_len_;

 public:
  CopyBytesToBufferTemplate(const uint8_t* source, const uint64_t source_len) :
    source_(source),
    source_len_(source_len) {
    ASSERT(source_ && source_len_ != 0);
  }
  virtual ~CopyBytesToBufferTemplate() = default;

  auto data() const -> const uint8_t* {
    return source_;
  }

  auto size() const -> uint64_t {
    return source_len_;
  }
};

class CopyBytesToBuffer : public CopyBytesToBufferTemplate {
  DEFINE_NON_COPYABLE_TYPE(CopyBytesToBuffer);

 public:
  CopyBytesToBuffer(const uint8_t* source, const uint64_t source_len) :
    CopyBytesToBufferTemplate(source, source_len) {}
  template <std140::IsAligned T>
  explicit CopyBytesToBuffer(const T& source) :
    CopyBytesToBuffer((const uint8_t*)&source, sizeof(T)) {}
  ~CopyBytesToBuffer() override = default;
  void operator()(Buffer* dst) const;

 public:
  static inline void Copy(const uint8_t* bytes, const uint64_t num_bytes, Buffer* dest) {
    CopyBytesToBuffer copy(bytes, num_bytes);
    return copy(dest);
  }

  template <typename T>
  static inline void Copy(const T* values, const uint64_t num_values, Buffer* dest) {
    CopyBytesToBuffer copy((const uint8_t*)values, num_values * sizeof(T));
    return copy(dest);
  }

  template <typename T>
  static inline void Copy(const T& value, Buffer* dest) {
    return Copy(&value, 1, dest);
  }
};

class CopyBytesToBufferWithStaging : public CopyBytesToBufferTemplate {
  DEFINE_NON_COPYABLE_TYPE(CopyBytesToBufferWithStaging);

 public:
  CopyBytesToBufferWithStaging(const uint8_t* source, const uint64_t source_len) :
    CopyBytesToBufferTemplate(source, source_len) {}
  CopyBytesToBufferWithStaging(const void* source, const uint64_t source_len) :
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    CopyBytesToBufferWithStaging((const uint8_t*)source, source_len) {}
  template <typename T>
  CopyBytesToBufferWithStaging(const T& source) :
    CopyBytesToBufferWithStaging((const uint8_t*)&source, sizeof(T)) {}
  ~CopyBytesToBufferWithStaging() override = default;
  void operator()(Buffer* dst) const;

 public:
  static inline void Copy(const uint8_t* bytes, const uint64_t num_bytes, Buffer* dest) {
    CopyBytesToBufferWithStaging copy(bytes, num_bytes);
    return copy(dest);
  }

  template <typename T>
  static inline void Copy(const T* values, const uint64_t num_values, Buffer* dest) {
    return Copy((const uint8_t*)values, num_values * sizeof(T), dest);
  }

  template <typename T>
  static inline void Copy(const T& value, Buffer* dest) {
    return Copy(&value, 1, dest);
  }
};

class CopyBufferToBuffer {
 private:
  VkBufferCopy copy_{};
  Buffer* source_;

 public:
  explicit CopyBufferToBuffer(Buffer* source, const uint64_t num_bytes = VK_WHOLE_SIZE,
                              const uint64_t source_offset = 0, const uint64_t dest_offset = 0);
  ~CopyBufferToBuffer() = default;

  void operator()(Buffer* dst) const;

 public:
  static inline void Copy(Buffer* source, Buffer* dest) {
    CopyBufferToBuffer copy(source);
    return copy(dest);
  }
};
}  // namespace prt::vk

#endif  // PRT_COPY_TO_BUFFER_H
