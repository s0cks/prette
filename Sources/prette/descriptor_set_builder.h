#ifndef PRT_DESCRIPTOR_SET_BUILDER_H
#define PRT_DESCRIPTOR_SET_BUILDER_H

#include <string>
#include <utility>
#include <vector>

#include "prette/common.h"
#include "prette/vk.h"

namespace prt::vk {
class DescriptorSetLayoutBindingBuilder {};

class DescriptorSetBuilder {
 public:
  static constexpr const auto kDefaultBindingStages = VK_SHADER_STAGE_VERTEX_BIT;
  using LayoutBinding = VkDescriptorSetLayoutBinding;
  using LayoutBindingList = std::vector<LayoutBinding>;

  class LayoutBindingBuilder {
    DEFINE_DEFAULT_COPYABLE_TYPE(LayoutBindingBuilder);

   private:
    LayoutBinding* binding_;

   public:
    explicit LayoutBindingBuilder(LayoutBinding* binding) :
      binding_(binding) {}
    ~LayoutBindingBuilder() = default;

    inline auto binding() const -> LayoutBinding* {
      return binding_;
    }

    auto SetBinding(const uint64_t rhs) -> LayoutBindingBuilder& {
      binding_->binding = rhs;
      return *this;
    }

    auto SetDescriptorType(const VkDescriptorType rhs) -> LayoutBindingBuilder& {
      binding_->descriptorType = rhs;
      return *this;
    }

    auto SetDescriptorCount(const uint64_t rhs) -> LayoutBindingBuilder& {
      binding_->descriptorCount = rhs;
      return *this;
    }

    auto SetStageFlags(const VkShaderStageFlags rhs) -> LayoutBindingBuilder& {
      binding_->stageFlags = rhs;
      return *this;
    }

    auto SetImmutableSamplers(const VkSampler* rhs) -> LayoutBindingBuilder& {
      binding_->pImmutableSamplers = rhs;
      return *this;
    }
  };

 private:
  std::string name_{};
  LayoutBindingList bindings_{};

 public:
  explicit DescriptorSetBuilder(const uint64_t num_bindings = 0) :
    bindings_() {
    if (num_bindings > 0)
      bindings_.reserve(num_bindings);
  }
  ~DescriptorSetBuilder() = default;

  auto GetName() const -> const std::string& {
    return name_;
  }

  auto WithName(const std::string rhs) -> DescriptorSetBuilder& {
    ASSERT(!rhs.empty());
    name_ = std::move(rhs);
    return *this;
  }

  auto GetBindings() const -> const LayoutBindingList& {
    return bindings_;
  }

  auto AddBinding(const VkDescriptorType type, const VkShaderStageFlags stages) -> LayoutBindingBuilder {
    const auto idx = bindings_.size();
    bindings_.resize(idx + 1);
    auto& binding = bindings_[idx];
    binding.binding = idx;
    binding.descriptorType = type;
    binding.descriptorCount = 1;
    binding.stageFlags = stages;
    binding.pImmutableSamplers = nullptr;
    return LayoutBindingBuilder(&binding);
  }

  inline auto AddUniformBufferBinding(const VkShaderStageFlags stages = kDefaultBindingStages) -> LayoutBindingBuilder {
    return AddBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, stages);
  }

  inline auto AddSamplerBinding(const VkShaderStageFlags stages = kDefaultBindingStages) -> LayoutBindingBuilder {
    return AddBinding(VK_DESCRIPTOR_TYPE_SAMPLER, stages);
  }

  inline auto AddCombinedImageSamplerBinding(const VkShaderStageFlags stages = kDefaultBindingStages)
      -> LayoutBindingBuilder {
    return AddBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stages);
  }

  auto Build() -> DescriptorSet*;

  auto operator()() -> DescriptorSet* {
    return Build();
  }

  operator DescriptorSet*() {
    return Build();
  }
};
}  // namespace prt::vk

#endif  // PRT_DESCRIPTOR_SET_BUILDER_H
