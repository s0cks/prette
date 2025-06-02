#ifndef PRT_DESCRIPTOR_SET_LAYOUT_H
#define PRT_DESCRIPTOR_SET_LAYOUT_H

#include <cstdint>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/vk.h"

namespace prt::vk {
class DescriptorSetLayout : public vk::HandleTemplate<VkDescriptorSetLayout> {
 public:
  DescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo* create_info);
  ~DescriptorSetLayout() override;
  auto ToString() const -> std::string override;

  operator VkDescriptorSetLayout() {
    return handle_ref();
  }
};

class DescriptorSetLayoutBuilder :
  public vk::HandleBuilderTemplate<VkDescriptorSetLayoutCreateInfo, DescriptorSetLayout> {
 public:
  class BindingBuilder {
    friend class DescriptorSetLayoutBuilder;
    DEFINE_DEFAULT_COPYABLE_TYPE(BindingBuilder);

   private:
    VkDescriptorSetLayoutBinding* binding_;

    explicit BindingBuilder(VkDescriptorSetLayoutBinding* binding, const uint64_t idx) :
      binding_(binding) {
      ASSERT(binding_);
      binding_ptr()->binding = idx;
      binding_ptr()->descriptorCount = 1;
      binding_ptr()->pImmutableSamplers = nullptr;
    }

    inline auto binding_ptr() -> VkDescriptorSetLayoutBinding* {
      return binding_;
    }

   public:
    ~BindingBuilder() = default;

    auto WithDescriptorType(const VkDescriptorType rhs) -> BindingBuilder& {
      binding_ptr()->descriptorType = rhs;
      return *this;
    }

    auto WithDescriptorCount(const uint64_t rhs) -> BindingBuilder& {
      binding_ptr()->descriptorCount = rhs;
      return *this;
    }

    auto WithStageFlags(const VkShaderStageFlags rhs) -> BindingBuilder& {
      binding_ptr()->stageFlags = rhs;
      return *this;
    }

    auto WithImmutableSamplers(const VkSampler* rhs) -> BindingBuilder& {
      binding_ptr()->pImmutableSamplers = rhs;
      return *this;
    }
  };

 private:
  std::vector<VkDescriptorSetLayoutBinding> bindings_{};

 public:
  DescriptorSetLayoutBuilder(const uint64_t num_bindings = 0) :
    vk::HandleBuilderTemplate<VkDescriptorSetLayoutCreateInfo, DescriptorSetLayout>() {
    bindings_.reserve(num_bindings > 0 ? num_bindings : 10);
    info_ptr()->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  }
  ~DescriptorSetLayoutBuilder() override = default;

  auto WithNext(const void* rhs) -> DescriptorSetLayoutBuilder& {
    ASSERT(rhs);
    info_ptr()->pNext = rhs;
    return *this;
  }

  auto WithFlags(const VkDescriptorSetLayoutCreateFlags rhs) -> DescriptorSetLayoutBuilder& {
    info_ptr()->flags = rhs;
    return *this;
  }

  auto AddBinding() -> BindingBuilder {
    const auto idx = bindings_.size();
    bindings_.resize(idx + 1);
    return BindingBuilder(&bindings_[idx], idx);
  }

  auto AddBinding(const VkDescriptorType type) -> BindingBuilder {
    const auto idx = bindings_.size();
    bindings_.resize(idx + 1);
    // clang-format off
    return BindingBuilder(&bindings_[idx], idx)
      .WithDescriptorType(type);
    // clang-format on
  }

  inline auto AddSamplerBinding() -> BindingBuilder {
    return AddBinding(VK_DESCRIPTOR_TYPE_SAMPLER);
  }

  inline auto AddCombinedImageSamplerBinding() -> BindingBuilder {
    return AddBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
  }

  inline auto AddUniformBufferBinding() -> BindingBuilder {
    return AddBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
  }

  inline auto AddDynamicUniformBufferBinding() -> BindingBuilder {
    return AddBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);
  }

  inline auto AddStorageBufferBinding() -> BindingBuilder {
    return AddBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
  }

  inline auto AddDynamicStorageBufferBinding() -> BindingBuilder {
    return AddBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC);
  }

  inline auto AddInputAttachmentBinding() -> BindingBuilder {
    return AddBinding(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);
  }

  inline auto AddInlineUniformBlockBinding() -> BindingBuilder {
    return AddBinding(VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK);
  }

  auto IsValid() const -> bool override {
    return bindings_.size() > 0;
  }

  auto Build() -> DescriptorSetLayout* override;

  auto operator()() -> DescriptorSetLayout* {
    return Build();
  }

  operator DescriptorSetLayout*() {
    return Build();
  }
};
}  // namespace prt::vk

#endif  // PRT_DESCRIPTOR_SET_LAYOUT_H
