#ifndef PRT_RENDER_PASS_BUILDER_H
#define PRT_RENDER_PASS_BUILDER_H

#include <string>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/platform.h"
#include "prette/vk.h"

namespace prt::vk {

class RenderPassBuilder : public NamedHandleBuilderTemplate<VkRenderPassCreateInfo, RenderPass, RenderPassBuilder> {
 private:
  template <typename V>
  class ChildBuilderTemplate {
   public:
    using Id = uint64_t;
    using ValuePtr = V*;

   private:
    Id id_;
    ValuePtr value_;

   protected:
    ChildBuilderTemplate(const Id id, ValuePtr value) :
      id_(id),
      value_(value) {}

    inline auto value_ptr() const -> ValuePtr {
      return value_;
    }

   public:
    virtual ~ChildBuilderTemplate() = default;

    auto GetId() const -> Id {
      return id_;
    }
  };

 public:
  class AttachmentDescBuilder : public ChildBuilderTemplate<VkAttachmentDescription> {
    friend class RenderPassBuilder;
    DEFINE_DEFAULT_COPYABLE_TYPE(AttachmentDescBuilder);

   private:
    explicit AttachmentDescBuilder(const Id id, ValuePtr value) :
      ChildBuilderTemplate(id, value) {
      value_ptr()->format = VK_FORMAT_UNDEFINED;
      value_ptr()->samples = VK_SAMPLE_COUNT_1_BIT;
      value_ptr()->loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      value_ptr()->storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      value_ptr()->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      value_ptr()->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      value_ptr()->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      value_ptr()->finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }

   public:
    ~AttachmentDescBuilder() = default;

    auto WithSamples(const VkSampleCountFlagBits rhs) -> AttachmentDescBuilder& {
      value_ptr()->samples = rhs;
      return *this;
    }

    auto WithFormat(const VkFormat rhs) -> AttachmentDescBuilder& {
      value_ptr()->format = rhs;
      return *this;
    }

    auto WithLoadOp(const VkAttachmentLoadOp rhs) -> AttachmentDescBuilder& {
      value_ptr()->loadOp = rhs;
      return *this;
    }

    inline auto WithLoadOpClear() -> AttachmentDescBuilder& {
      return WithLoadOp(VK_ATTACHMENT_LOAD_OP_CLEAR);
    }

    inline auto WithLoadOpLoad() -> AttachmentDescBuilder& {
      return WithLoadOp(VK_ATTACHMENT_LOAD_OP_LOAD);
    }

    inline auto WithLoadOpDontCare() -> AttachmentDescBuilder& {
      return WithLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE);
    }

    auto WithStoreOp(const VkAttachmentStoreOp rhs) -> AttachmentDescBuilder& {
      value_ptr()->storeOp = rhs;
      return *this;
    }

    inline auto WithStoreOpNone() -> AttachmentDescBuilder& {
      return WithStoreOp(VK_ATTACHMENT_STORE_OP_NONE);
    }

    inline auto WithStoreOpStore() -> AttachmentDescBuilder& {
      return WithStoreOp(VK_ATTACHMENT_STORE_OP_STORE);
    }

    inline auto WithStoreOpDontCare() -> AttachmentDescBuilder& {
      return WithStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE);
    }

    auto WithStencilLoadOp(const VkAttachmentLoadOp rhs) -> AttachmentDescBuilder& {
      value_ptr()->stencilLoadOp = rhs;
      return *this;
    }

    auto WithStencilStoreOp(const VkAttachmentStoreOp rhs) -> AttachmentDescBuilder& {
      value_ptr()->stencilStoreOp = rhs;
      return *this;
    }

    auto WithInitialLayout(const VkImageLayout rhs) -> AttachmentDescBuilder& {
      value_ptr()->initialLayout = rhs;
      return *this;
    }

    inline auto WithInitialLayoutUndefined() -> AttachmentDescBuilder& {
      return WithInitialLayout(VK_IMAGE_LAYOUT_UNDEFINED);
    }

    auto WithFinalLayout(const VkImageLayout rhs) -> AttachmentDescBuilder& {
      value_ptr()->finalLayout = rhs;
      return *this;
    }

    inline auto WithFinalLayoutUndefined() -> AttachmentDescBuilder& {
      return WithFinalLayout(VK_IMAGE_LAYOUT_UNDEFINED);
    }

    inline auto WithFinalLayoutShaderReadOptimal() -> AttachmentDescBuilder& {
      return WithFinalLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    inline auto WithFinalLayoutDepthStecilOptimal() -> AttachmentDescBuilder& {
      return WithFinalLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    }

    auto Build(const VkImageLayout ref_layout = VK_IMAGE_LAYOUT_UNDEFINED) const -> VkAttachmentReference {
      return {
          .attachment = static_cast<uint32_t>(GetId()),
          .layout = ref_layout != VK_IMAGE_LAYOUT_UNDEFINED ? ref_layout : value_ptr()->finalLayout,
      };
    }

    inline auto BuildWithColorAttachmentOptimalRef() const -> VkAttachmentReference {
      return Build(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    }
  };

  class SubpassBuilder : public ChildBuilderTemplate<VkSubpassDescription> {
    friend class RenderPassBuilder;
    DEFINE_DEFAULT_COPYABLE_TYPE(SubpassBuilder);

   private:
    std::vector<VkAttachmentReference> color_attachments_{};

    bool depth_ = false;
    VkAttachmentReference depth_stencil_{};

   private:
    explicit SubpassBuilder(const Id id, ValuePtr value) :
      ChildBuilderTemplate<VkSubpassDescription>(id, value) {
      color_attachments_.reserve(2);
      value_ptr()->colorAttachmentCount = 0;
      value_ptr()->pColorAttachments = nullptr;
      value_ptr()->pDepthStencilAttachment = nullptr;
      value_ptr()->inputAttachmentCount = 0;
      value_ptr()->pInputAttachments = nullptr;
      value_ptr()->preserveAttachmentCount = 0;
      value_ptr()->pPreserveAttachments = nullptr;
      value_ptr()->pResolveAttachments = nullptr;
    }

   public:
    ~SubpassBuilder() override = default;

    auto WithColorAttachment(const VkAttachmentReference& ref) -> SubpassBuilder& {
      color_attachments_.push_back(ref);
      return *this;
    }

    auto WithDepthAttachment(const VkAttachmentReference& rhs) -> SubpassBuilder& {
      depth_ = true;
      depth_stencil_ = rhs;
      return *this;
    }

    auto Bind(const VkPipelineBindPoint rhs) -> VkSubpassDescription* {
      value_ptr()->pipelineBindPoint = rhs;
      value_ptr()->colorAttachmentCount = color_attachments_.size();
      value_ptr()->pColorAttachments = color_attachments_.data();
      value_ptr()->pDepthStencilAttachment = depth_ ? &depth_stencil_ : nullptr;
      value_ptr()->inputAttachmentCount = 0;
      value_ptr()->pInputAttachments = nullptr;
      value_ptr()->preserveAttachmentCount = 0;
      value_ptr()->pPreserveAttachments = nullptr;
      value_ptr()->pResolveAttachments = nullptr;
      return value_ptr();
    }

    inline auto BindGraphics() -> VkSubpassDescription* {
      return Bind(VK_PIPELINE_BIND_POINT_GRAPHICS);
    }
  };

  class SubpassDepBuilder : public ChildBuilderTemplate<VkSubpassDependency> {
    friend class RenderPassBuilder;
    DEFINE_DEFAULT_COPYABLE_TYPE(SubpassDepBuilder);

   public:
    struct SubpassDepMeta {
      uint32_t subpass;
      VkPipelineStageFlags stage;
      VkAccessFlags access;
    };

   private:
    explicit SubpassDepBuilder(const Id id, ValuePtr value) :
      ChildBuilderTemplate(id, value) {
      value_ptr()->srcSubpass = 0;
      value_ptr()->dstSubpass = 0;
      value_ptr()->srcStageMask = 0;
      value_ptr()->dstStageMask = 0;
      value_ptr()->srcAccessMask = 0;
      value_ptr()->dstAccessMask = 0;
      value_ptr()->dependencyFlags = 0;
    }

   public:
    ~SubpassDepBuilder() = default;

    auto WithSource(const SubpassDepMeta rhs) -> SubpassDepBuilder& {
      value_ptr()->srcSubpass = rhs.subpass;
      value_ptr()->srcStageMask = rhs.stage;
      value_ptr()->srcAccessMask = rhs.access;
      return *this;
    }

    auto WithDest(const SubpassDepMeta rhs) -> SubpassDepBuilder& {
      value_ptr()->dstSubpass = rhs.subpass;
      value_ptr()->dstStageMask = rhs.stage;
      value_ptr()->dstAccessMask = rhs.access;
      return *this;
    }

    auto WithDependencyFlags(const VkDependencyFlags rhs) -> SubpassDepBuilder& {
      value_ptr()->dependencyFlags = rhs;
      return *this;
    }

    inline auto WithDependencyByRegion() -> SubpassDepBuilder& {
      return WithDependencyFlags(VK_DEPENDENCY_BY_REGION_BIT);
    }
  };

 private:
  std::string name_{};
  std::vector<VkAttachmentDescription> attachments_{};
  std::vector<VkSubpassDependency> subpass_deps_{};
  std::vector<VkSubpassDescription> subpasses_{};

  inline void UpdateInfo() {
    info_ptr()->attachmentCount = GetNumberOfAttachments();
    info_ptr()->pAttachments = attachments();
    info_ptr()->subpassCount = GetNumberOfSubpasses();
    info_ptr()->pSubpasses = subpasses();
    info_ptr()->dependencyCount = GetNumberOfSubpassDependencies();
    info_ptr()->pDependencies = subpass_dependencies();
  }

 public:
  RenderPassBuilder();
  ~RenderPassBuilder();

  auto WithName(const std::string rhs) -> RenderPassBuilder& {
    ASSERT_NOT_EMPTY(rhs);
    name_ = std::move(rhs);
    return *this;
  }

  auto AddAttachment() -> AttachmentDescBuilder {
    const auto idx = GetNumberOfAttachments();
    attachments_.resize(idx + 1);
    return AttachmentDescBuilder(idx, &attachments_[idx]);
  }

  auto GetNumberOfAttachments() const -> uint64_t {
    return attachments_.size();
  }

  auto HasAttachments() const -> bool {
    return !attachments_.empty();
  }

  auto attachments() const -> VkAttachmentDescription const* {
    return attachments_.data();
  }

  auto AddSubpassDependency() -> SubpassDepBuilder {
    const auto idx = GetNumberOfSubpassDependencies();
    subpass_deps_.resize(idx + 1);
    return SubpassDepBuilder(idx, &subpass_deps_[idx]);
  }

  auto GetNumberOfSubpassDependencies() const -> uint64_t {
    return subpass_deps_.size();
  }

  auto HasSubpassDependencies() const -> bool {
    return !subpass_deps_.empty();
  }

  auto subpass_dependencies() const -> VkSubpassDependency const* {
    return subpass_deps_.data();
  }

  auto AddSubpass() -> SubpassBuilder {
    const auto idx = GetNumberOfSubpasses();
    subpasses_.resize(idx + 1);
    return SubpassBuilder(idx, &subpasses_[idx]);
  }

  auto GetNumberOfSubpasses() const -> uint64_t {
    return subpasses_.size();
  }

  auto HasSubpasses() const -> bool {
    return !subpasses_.empty();
  }

  auto subpasses() const -> VkSubpassDescription const* {
    return subpasses_.data();
  }

  auto IsValid() const -> bool override;
  auto Build() -> RenderPass* override;

  template <typename T>
  auto BuildTyped() -> T* {
    UpdateInfo();
    return new T(info_ptr());
  }

  template <typename T>
  auto BuildNamedType() -> T* {
    UpdateInfo();
    return new T(name_, info_ptr());
  }
};
}  // namespace prt::vk

#endif  // PRT_RENDER_PASS_BUILDER_H
