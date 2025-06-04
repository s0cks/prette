#ifndef PRT_VK_H
#define PRT_VK_H

#include <functional>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

// IWYU pragma: begin_exports
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
// IWYU pragma: end_exports

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/relaxed_atomic.h"
#include "prette/traits.h"

namespace prt {
class Driver;
class Framebuffer;
namespace vk {
class Surface;

class Instance;
using InstancePtr = std::unique_ptr<Instance>;

class PhysicalDevice;
using PhysicalDevicePtr = std::unique_ptr<PhysicalDevice>;

class Device;
using DevicePtr = std::unique_ptr<Device>;

using Extension = std::string;
using ExtensionList = std::vector<Extension>;
using ExtensionSet = std::unordered_set<Extension>;

using Layer = std::string;
using LayerList = std::vector<Layer>;
using LayerSet = std::unordered_set<Layer>;

class Buffer;
class Shader;
class Image;
class ImageView;
class Pipeline;
class PipelineLayout;
class PipelineCache;
class RenderPass;
class CommandPool;
class DescriptorSet;

class Result {
 private:
  VkResult value_;

 public:
  Result(const VkResult value = VK_SUCCESS) :
    value_(value) {}
  ~Result() = default;

  auto value() const -> const VkResult& {
    return value_;
  }

  auto IsSuccess() const -> bool {
    return value() == VK_SUCCESS;
  }

  auto IsSubOptimal() const -> bool {
    return value() == VK_SUBOPTIMAL_KHR;
  }

  auto IsOutOfDate() const -> bool {
    return value() == VK_ERROR_OUT_OF_DATE_KHR;
  }

  auto IsOutOfDeviceMemory() const -> bool {
    return value() == VK_ERROR_OUT_OF_DEVICE_MEMORY;
  }

  auto IsOutOfPoolMemory() const -> bool {
    return value() == VK_ERROR_OUT_OF_POOL_MEMORY || value() == VK_ERROR_OUT_OF_POOL_MEMORY_KHR;
  }

  operator VkResult() const {
    return value();
  }

  operator bool() const {
    return IsSuccess();
  }

  friend auto operator<<(std::ostream& stream, const Result& rhs) -> std::ostream& {
    return stream << std::string(string_VkResult(rhs));
  }
};

template <typename H>
class HandleTemplate {
  using HandleType = H;

 private:
  HandleType handle_;

 protected:
  HandleTemplate(const HandleType handle = VK_NULL_HANDLE) :
    handle_(handle) {}

  auto handle_ref() -> HandleType& {
    return handle_;
  }

  auto handle_ptr() -> HandleType* {
    return &handle_;
  }

  void SetHandle(const HandleType handle) {
    ASSERT(handle != VK_NULL_HANDLE);
    handle_ = handle;
  }

 public:
  virtual ~HandleTemplate<H>() = default;
  virtual auto ToString() const -> std::string = 0;

  auto GetHandle() const -> const HandleType& {
    return handle_;
  }

  inline auto IsInitialized() const -> bool {
    return GetHandle() != VK_NULL_HANDLE;
  }
};

template <AnyVkHandleType T>
static inline auto IsInitialized(const T* value) -> bool {
  return value && value->IsInitialized();
}

template <AnyVkHandleType... Ts>
static inline auto AllInitialized(const Ts*... values) -> bool {
  return (IsInitialized(values) && ...);
}

template <typename I, typename T>
class HandleBuilderTemplate {
 private:
  I info_{};

 protected:
  HandleBuilderTemplate() = default;

  auto info_ptr() -> I* {
    return &info_;
  }

 public:
  virtual ~HandleBuilderTemplate() = default;

  auto info() const -> const I& {
    return info_;
  }

  virtual auto IsValid() const -> bool = 0;
  virtual auto Build() -> T* = 0;
};

template <typename H>
class NamedHandleTemplate : public HandleTemplate<H> {
  using HandleType = H;

 private:
  std::string name_;

 protected:
  explicit NamedHandleTemplate(const std::string name, const HandleType handle = VK_NULL_HANDLE) :
    HandleTemplate<H>(handle),
    name_(std::move(name)) {}

  void SetName(const std::string rhs) {
    ASSERT_NOT_EMPTY(rhs);
    name_ = std::move(rhs);
  }

 public:
  virtual ~NamedHandleTemplate() = default;

  auto GetName() const -> const std::string& {
    return name_;
  }

  inline auto IsNamed(const char* rhs) const -> bool {
    ASSERT(rhs && strlen(rhs) > 0);
    return EqualsIgnoreCase(name_, rhs);
  }
};

template <typename I, typename T, typename B>
class NamedHandleBuilderTemplate : public HandleBuilderTemplate<I, T> {
 private:
  std::string name_{};

 protected:
  NamedHandleBuilderTemplate() = default;

 public:
  virtual ~NamedHandleBuilderTemplate() = default;

  auto GetName() const -> const std::string& {
    return name_;
  }

  auto WithName(const std::string rhs) -> B& {
    ASSERT_NOT_EMPTY(rhs);
    name_ = std::move(rhs);
    return (B&)*this;
  }

  inline auto HasName() const -> bool {
    return !name_.empty();
  }

  virtual auto IsValid() const -> bool {
    return HasName();
  }
};

class BaseFinalizer {
 private:
  RelaxedAtomic<uint64_t> num_finalized_ = 0;

 protected:
  BaseFinalizer() = default;

  inline void IncrementFinalizedCounter() {
    num_finalized_ += 1;
  }

  inline void DecrementFinalizedCounter() {
    num_finalized_ -= 1;
  }

 public:
  virtual ~BaseFinalizer() = default;

  auto GetNumberOfObjectsFinalized() const -> uint64_t {
    return (uint64_t)num_finalized_;
  }
};

static inline auto CalcArea(const VkExtent2D& rhs) -> uint64_t {
  return rhs.width * rhs.height;
}

static inline auto CalcImageArea(const VkExtent2D& rhs, const int num_channels = 4) -> uint64_t {
  return CalcArea(rhs) * num_channels;
}

static inline auto CalcArea(const VkExtent3D& rhs) -> uint64_t {
  return rhs.width * rhs.height;
}

#define VkOffset2DTo3D(Offset) \
  (VkOffset3D{                 \
      .x = (Offset).x,         \
      .y = (Offset).y,         \
      .z = 0,                  \
  })

#define VkExtent2DTo3D(Extent)   \
  (VkExtent3D{                   \
      .width = (Extent).width,   \
      .height = (Extent).height, \
      .depth = 1,                \
  })

using FormatPredicate = std::function<bool(const VkFormat&)>;

static constexpr const auto kDefaultDevicePriority = 1.0f;
}  // namespace vk
}  // namespace prt

#endif  // PRT_VK_H
