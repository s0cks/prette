#ifndef PRT_DESCRIPTOR_SET_H
#define PRT_DESCRIPTOR_SET_H

#include <functional>
#include <string>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/vk.h"

// IWYU pragma: begin_exports
#include "prette/descriptor_set_builder.h"
// IWYU pragma: end_exports

namespace prt::vk {
using DescriptorSetPredicate = std::function<bool(DescriptorSet*)>;

class DescriptorSetVisitor {
 protected:
  DescriptorSetVisitor() = default;

 public:
  virtual ~DescriptorSetVisitor() = default;
  virtual auto Visit(DescriptorSet* rhs) -> bool = 0;
};

class DescriptorSet : public NamedHandleTemplate<VkDescriptorSet> {
 private:
  VkDescriptorSetLayout layout_ = VK_NULL_HANDLE;

  static void InitLayout(const VkDescriptorSetLayoutBinding* bindings, const uint64_t num_bindings,
                         VkDescriptorSetLayout& layout);
  static void AllocSets(const VkDescriptorSetLayout& layout, VkDescriptorSet* sets, const uint64_t num_sets);

 public:
  explicit DescriptorSet(std::string name, VkDescriptorSetLayout layout);
  ~DescriptorSet();

  auto GetLayout() const -> const VkDescriptorSetLayout& {
    return layout_;
  }

  template <typename WritesContainer>
  inline void Update(const WritesContainer& writes) {
    return Update(writes.data(), writes.size());
  }

  auto ToString() const -> std::string override;
  void Update(const VkWriteDescriptorSet* writes, const uint64_t num_writes);

  operator VkDescriptorSet() const {
    return GetHandle();
  }

 public:
  static auto New(const std::string name, const VkDescriptorSetLayout layout) -> DescriptorSet*;
  static auto New(const std::string name, const VkDescriptorSetLayoutBinding* bindings, const uint64_t num_bindings)
      -> DescriptorSet*;

  template <typename BindingsContainer>
  static inline auto New(const std::string name, const BindingsContainer& bindings) -> DescriptorSet* {
    return New(std::move(name), bindings.data(), bindings.size());
  }

  static auto FromJsonFile(const fs::path path) -> DescriptorSet*;
};

void BindDescriptorSet(VkCommandBuffer buffer, DescriptorSet* dset, PipelineLayout* layout);

class RenderPipeline;
void BindDescriptorSet(VkCommandBuffer buffer, DescriptorSet* dset, RenderPipeline* pipeline);

class DescriptorSetFinalizer : public BaseFinalizer, public DescriptorSetVisitor {
 public:
  DescriptorSetFinalizer() = default;
  ~DescriptorSetFinalizer() override = default;
  auto Visit(DescriptorSet* rhs) -> bool override;

 public:
  static void FinalizeAll();
  static void FinalizeAll(const std::vector<DescriptorSet*>& rhs);
};

void InitDescriptorSets();
auto FindDescriptorSet(const std::string name) -> DescriptorSet*;
auto VisitAllDescriptorSets(DescriptorSetVisitor* vis) -> bool;
auto VisitAllDescriptorSets(const std::function<bool(DescriptorSet*)>& vis) -> bool;
}  // namespace prt::vk

#endif  // PRT_DESCRIPTOR_SET_H
