#include "prette/descriptor_set.h"

#include <cstddef>
#include <fmt/format.h>
#include <functional>
#include <glog/logging.h>
#include <string>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/device.h"  // IWYU pragma: keep
#include "prette/gfx.h"
#include "prette/gfx_driver.h"
#include "prette/gfx_driver_event.h"
#include "prette/pipeline/pipeline.h"
#include "prette/pipeline/pipeline_layout.h"
#include "prette/to_string.h"
#include "prette/vk.h"
#include "prette/vk_buffer.h"

namespace prt::vk {
static std::vector<DescriptorSet*> all_{};

void DescriptorSet::InitLayout(const VkDescriptorSetLayoutBinding* bindings, const uint64_t num_bindings,
                               VkDescriptorSetLayout& layout) {
  ASSERT(num_bindings > 0);
  const auto driver = Driver::Get();
  VkDescriptorSetLayoutCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  create_info.bindingCount = num_bindings;
  create_info.pBindings = bindings;
  CHECK_VK(FATAL, vkCreateDescriptorSetLayout(*driver->GetDevice(), &create_info, driver->GetAllocator(), &layout),
           "failed to create VkDescriptorSetLayout");
}

void DescriptorSet::AllocSets(const VkDescriptorSetLayout& layout, VkDescriptorSet* dsets, const uint64_t num_sets) {
  const auto driver = Driver::Get();
  VkDescriptorSetAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = driver->GetDescriptorPool();
  alloc_info.descriptorSetCount = num_sets;
  alloc_info.pSetLayouts = &layout;
  CHECK_VK(FATAL, vkAllocateDescriptorSets(*driver->GetDevice(), &alloc_info, dsets),
           fmt::format("failed to allocate {} VkDescriptorSets", num_sets));
}

DescriptorSet::DescriptorSet(const std::string name, const VkDescriptorSetLayout layout) :
  NamedHandleTemplate<VkDescriptorSet>(std::move(name)),
  layout_(layout) {
  AllocSets(layout_, handle_ptr(), 1);
}

DescriptorSet::~DescriptorSet() {
  const auto driver = Driver::Get();
  driver->DestroyDescriptorSetLayout(layout_);
  vkFreeDescriptorSets(*driver->GetDevice(), driver->GetDescriptorPool(), 1, handle_ptr());
}

auto DescriptorSet::ToString() const -> std::string {
  return ToStringHelper<DescriptorSet>{};
}

void DescriptorSet::Update(const VkWriteDescriptorSet* writes, const uint64_t num_writes) {
  ASSERT(num_writes > 0);
  const auto driver = Driver::Get();
  vkUpdateDescriptorSets(*driver->GetDevice(), num_writes, writes, 0, nullptr);
}

auto DescriptorSet::New(const std::string name, const VkDescriptorSetLayout layout) -> DescriptorSet* {
  const auto dset = new DescriptorSet(std::move(name), layout);
  all_.push_back(dset);
  return dset;
}

auto DescriptorSet::New(const std::string name, const VkDescriptorSetLayoutBinding* bindings,
                        const uint64_t num_bindings) -> DescriptorSet* {
  ASSERT(num_bindings > 0);
  VkDescriptorSetLayout layout = VK_NULL_HANDLE;
  InitLayout(bindings, num_bindings, layout);
  return New(std::move(name), std::move(layout));
}

void InitDescriptorSets() {
  OnDestroyingDriverEvent().subscribe([](DestroyingDriverEvent* event) {
    all_.clear();
  });
}

auto FindDescriptorSet(const std::string name) -> DescriptorSet* {
  ASSERT(!name.empty());
  for (const auto& dset : all_) {
    ASSERT(dset);
    if (EqualsIgnoreCase(name, dset->GetName()))
      return dset;
  }
  return nullptr;
}

auto VisitAllDescriptorSets(DescriptorSetVisitor* vis) -> bool {
  ASSERT(vis);
  for (const auto& dset : all_) {
    ASSERT(dset);
    if (!vis->Visit(dset))
      return false;
  }
  return true;
}

auto VisitAllDescriptorSets(const std::function<bool(DescriptorSet*)>& vis) -> bool {
  for (const auto& dset : all_) {
    ASSERT(dset);
    if (!vis(dset))
      return false;
  }
  return true;
}

auto DescriptorSetFinalizer::Visit(DescriptorSet* rhs) -> bool {
  ASSERT(rhs);
  DVLOG(2) << "finalizing: " << ((void*)rhs);
  delete rhs;
  IncrementFinalizedCounter();
  return true;
}

void DescriptorSetFinalizer::FinalizeAll() {
  std::vector<DescriptorSet*> descriptors{};
  descriptors.insert(std::end(descriptors), std::begin(all_), std::end(all_));
  return FinalizeAll(descriptors);
}

void DescriptorSetFinalizer::FinalizeAll(const std::vector<DescriptorSet*>& rhs) {
  DVLOG(1) << "finalizing DescriptorSets....";
  DescriptorSetFinalizer finalizer{};
  for (const auto& dset : rhs) {
    LOG_IF(FATAL, !finalizer.Visit(dset)) << "failed to finalize: " << ((void*)dset);
  }
  DVLOG(1) << "finalized " << finalizer.GetNumberOfObjectsFinalized() << " DescriptorSets";
}

void BindDescriptorSet(VkCommandBuffer buffer, DescriptorSet* dset, PipelineLayout* layout) {
  ASSERT_INITIALIZED(dset);
  ASSERT_INITIALIZED(layout);
  vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *layout, 0, 1, &dset->GetHandle(), 0, nullptr);
}

void BindDescriptorSet(VkCommandBuffer buffer, DescriptorSet* dset, RenderPipeline* pipeline) {
  ASSERT_INITIALIZED(dset);
  ASSERT_INITIALIZED(pipeline);
  return BindDescriptorSet(buffer, dset, pipeline->GetPipelineLayout());
}
}  // namespace prt::vk
