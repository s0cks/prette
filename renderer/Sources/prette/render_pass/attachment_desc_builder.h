#ifndef PRT_ATTACHMENT_DESC_BUILDER_H
#define PRT_ATTACHMENT_DESC_BUILDER_H

#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/common.h"

namespace prt {
#define FOR_EACH_LOAD_OP(V)                    \
  V(DontCare, VK_ATTACHMENT_LOAD_OP_DONT_CARE) \
  V(Clear, VK_ATTACHMENT_LOAD_OP_CLEAR)        \
  V(Load, VK_ATTACHMENT_LOAD_OP_LOAD)          \
  V(None, VK_ATTACHMENT_LOAD_OP_NONE_KHR)

#define FOR_EACH_STORE_OP(V)                    \
  V(DontCare, VK_ATTACHMENT_STORE_OP_DONT_CARE) \
  V(Store, VK_ATTACHMENT_STORE_OP_STORE)        \
  V(None, VK_ATTACHMENT_STORE_OP_NONE)

#define FOR_EACH_STENCIL_LOAD_OP(V)            \
  V(DontCare, VK_ATTACHMENT_LOAD_OP_DONT_CARE) \
  V(Clear, VK_ATTACHMENT_LOAD_OP_CLEAR)        \
  V(Load, VK_ATTACHMENT_LOAD_OP_LOAD)          \
  V(None, VK_ATTACHMENT_LOAD_OP_NONE_KHR)

#define FOR_EACH_STENCIL_STORE_OP(V)            \
  V(DontCare, VK_ATTACHMENT_STORE_OP_DONT_CARE) \
  V(Store, VK_ATTACHMENT_STORE_OP_STORE)        \
  V(None, VK_ATTACHMENT_STORE_OP_NONE)

#define FOR_EACH_INITIAL_LAYOUT(V)        \
  V(Undefined, VK_IMAGE_LAYOUT_UNDEFINED) \
  V(ColorAttachmentOptimal, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)

#define FOR_EACH_FINAL_LAYOUT(V)                                           \
  V(Undefined, VK_IMAGE_LAYOUT_UNDEFINED)                                  \
  V(ShaderReadOptimal, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)           \
  V(ColorAttachmentOptimal, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)      \
  V(DepthStencilOptimal, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) \
  V(PresentSource, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)                        \
  V(SharedPresent, VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR)

class AttachmentDescBuilder {
  friend class RenderPassBuilder;
  DEFINE_DEFAULT_COPYABLE_TYPE(AttachmentDescBuilder);

 public:
  static constexpr const VkFormat kDefaultFormat = VK_FORMAT_UNDEFINED;
  static constexpr const VkSampleCountFlagBits kDefaultNumberOfSamples = VK_SAMPLE_COUNT_1_BIT;
  static constexpr const VkAttachmentLoadOp kDefaultLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  static constexpr const VkAttachmentStoreOp kDefaultStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  static constexpr const VkAttachmentLoadOp kDefaultStencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  static constexpr const VkAttachmentStoreOp kDefaultStencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  static constexpr const VkImageLayout kDefaultInitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  static constexpr const VkImageLayout kDefaultFinalLayout = VK_IMAGE_LAYOUT_UNDEFINED;

 private:
  uint32_t index_;
  VkAttachmentDescription* desc_;

  inline auto desc_ptr() const -> VkAttachmentDescription* {
    return desc_;
  }

 public:
  explicit AttachmentDescBuilder(const uint32_t index, VkAttachmentDescription* desc) :
    index_(index),
    desc_(desc) {
    ASSERT(desc_);
    desc_ptr()->format = kDefaultFormat;
    desc_ptr()->samples = kDefaultNumberOfSamples;
    desc_ptr()->loadOp = kDefaultLoadOp;
    desc_ptr()->storeOp = kDefaultStoreOp;
    desc_ptr()->stencilLoadOp = kDefaultStencilLoadOp;
    desc_ptr()->stencilStoreOp = kDefaultStencilStoreOp;
    desc_ptr()->initialLayout = kDefaultInitialLayout;
    desc_ptr()->finalLayout = kDefaultFinalLayout;
  }
  ~AttachmentDescBuilder() = default;

  auto GetIndex() const -> uint32_t {
    return index_;
  }

  auto WithSamples(const VkSampleCountFlagBits rhs) -> AttachmentDescBuilder& {
    desc_ptr()->samples = rhs;
    return *this;
  }

  inline auto With1Samples() -> AttachmentDescBuilder& {
    return WithSamples(VK_SAMPLE_COUNT_1_BIT);
  }

  inline auto With2Samples() -> AttachmentDescBuilder& {
    return WithSamples(VK_SAMPLE_COUNT_2_BIT);
  }

  inline auto With4Samples() -> AttachmentDescBuilder& {
    return WithSamples(VK_SAMPLE_COUNT_4_BIT);
  }

  inline auto With8Samples() -> AttachmentDescBuilder& {
    return WithSamples(VK_SAMPLE_COUNT_8_BIT);
  }

  inline auto With16Samples() -> AttachmentDescBuilder& {
    return WithSamples(VK_SAMPLE_COUNT_16_BIT);
  }

  inline auto With32Samples() -> AttachmentDescBuilder& {
    return WithSamples(VK_SAMPLE_COUNT_32_BIT);
  }

  inline auto With64Samples() -> AttachmentDescBuilder& {
    return WithSamples(VK_SAMPLE_COUNT_64_BIT);
  }

  auto WithFormat(const VkFormat rhs) -> AttachmentDescBuilder& {
    desc_ptr()->format = rhs;
    return *this;
  }

  auto WithLoadOp(const VkAttachmentLoadOp rhs) -> AttachmentDescBuilder& {
    desc_ptr()->loadOp = rhs;
    return *this;
  }

#define DEFINE_LOAD_OP(Name, Value)                        \
  inline auto WithLoadOp##Name()->AttachmentDescBuilder& { \
    return WithLoadOp(Value);                              \
  }
  FOR_EACH_LOAD_OP(DEFINE_LOAD_OP)
#undef DEFINE_LOAD_OP

  auto WithStoreOp(const VkAttachmentStoreOp rhs) -> AttachmentDescBuilder& {
    desc_ptr()->storeOp = rhs;
    return *this;
  }

#define DEFINE_STORE_OP(Name, Value)                        \
  inline auto WithStoreOp##Name()->AttachmentDescBuilder& { \
    return WithStoreOp(Value);                              \
  }
  FOR_EACH_STORE_OP(DEFINE_STORE_OP)
#undef DEFINE_STORE_OP

  auto WithStencilLoadOp(const VkAttachmentLoadOp rhs) -> AttachmentDescBuilder& {
    desc_ptr()->stencilLoadOp = rhs;
    return *this;
  }

#define DEFINE_LOAD_OP(Name, Value)                               \
  inline auto WithStencilLoadOp##Name()->AttachmentDescBuilder& { \
    return WithStencilLoadOp(Value);                              \
  }
  FOR_EACH_STENCIL_LOAD_OP(DEFINE_LOAD_OP)
#undef DEFINE_LOAD_OP

  auto WithStencilStoreOp(const VkAttachmentStoreOp rhs) -> AttachmentDescBuilder& {
    desc_ptr()->stencilStoreOp = rhs;
    return *this;
  }

#define DEFINE_STORE_OP(Name, Value)                               \
  inline auto WithStencilStoreOp##Name()->AttachmentDescBuilder& { \
    return WithStencilStoreOp(Value);                              \
  }
  FOR_EACH_STENCIL_STORE_OP(DEFINE_STORE_OP)
#undef DEFINE_STORE_OP

  auto WithInitialLayout(const VkImageLayout rhs) -> AttachmentDescBuilder& {
    desc_ptr()->initialLayout = rhs;
    return *this;
  }

#define DEFINE_WITH_LAYOUT(Name, Value)                           \
  inline auto WithInitialLayout##Name()->AttachmentDescBuilder& { \
    return WithInitialLayout((Value));                            \
  }
  FOR_EACH_INITIAL_LAYOUT(DEFINE_WITH_LAYOUT)
#undef DEFINE_WITH_LAYOUT

  auto WithFinalLayout(const VkImageLayout rhs) -> AttachmentDescBuilder& {
    desc_ptr()->finalLayout = rhs;
    return *this;
  }

#define DEFINE_WITH_LAYOUT(Name, Value)                         \
  inline auto WithFinalLayout##Name()->AttachmentDescBuilder& { \
    return WithFinalLayout((Value));                            \
  }
  FOR_EACH_FINAL_LAYOUT(DEFINE_WITH_LAYOUT)
#undef DEFINE_WITH_LAYOUT

  auto Build(const VkImageLayout ref_layout = VK_IMAGE_LAYOUT_UNDEFINED) const -> VkAttachmentReference {
    const auto attachment = GetIndex();
    VkImageLayout layout = desc_ptr()->finalLayout;
    if (ref_layout != VK_IMAGE_LAYOUT_UNDEFINED)
      layout = ref_layout;
    return {.attachment = attachment, .layout = layout};
  }

  inline auto BuildWithColorAttachmentOptimalRef() const -> VkAttachmentReference {
    return Build(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  }

  inline auto BuildWithDepthStencilOptimalRef() const -> VkAttachmentReference {
    return Build(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  }
};

#undef FOR_EACH_LOAD_OP
#undef FOR_EACH_STORE_OP
#undef FOR_EACH_STENCIL_LOAD_OP
#undef FOR_EACH_STENCIL_STORE_OP
#undef FOR_EACH_FINAL_LAYOUT
}  // namespace prt

#endif  // PRT_ATTACHMENT_DESC_BUILDER_H
