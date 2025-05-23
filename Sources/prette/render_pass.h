#ifndef PRT_RENDER_PASS_H
#define PRT_RENDER_PASS_H

#include <array>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "prette/common.h"
#include "prette/descriptor_set.h"
#include "prette/rx.h"  // IWYU pragma: keep
#include "prette/vk.h"
#include "prette/vk_cmd_buffers.h"

namespace prt {
class Renderer;
namespace vk {
class RenderPipeline;
using RenderPassPredicate = std::function<bool(RenderPass*)>;

class RenderPassVisitor {
 protected:
  RenderPassVisitor() = default;

 public:
  virtual ~RenderPassVisitor() = default;
  virtual auto Visit(RenderPass* pass) -> bool = 0;
};

class RenderPass : public vk::NamedHandleTemplate<VkRenderPass> {
  friend class prt::Renderer;
  using HandleType = VkRenderPass;

 private:
  RenderPass* next_ = nullptr;
  RenderPipeline* pipeline_ = nullptr;
  DescriptorSet* descriptors_ = nullptr;
  vk::CommandBufferPool<>* command_buffers_ = nullptr;
  rx::subscription on_swap_init_{};
  rx::subscription on_swap_destroyed_{};

 protected:
  void SetDescriptors(DescriptorSet* rhs) {
    ASSERT_INITIALIZED(rhs);
    descriptors_ = rhs;
  }

  inline auto GetCommandBuffer(const uint32_t idx) const -> VkCommandBuffer* {
    return &command_buffers_->At(idx);
  }

  virtual void Execute() {
    // do nothing
  }

  void SetPipeline(RenderPipeline* rhs);
  virtual void OnSwapInit(const bool reinit);
  virtual void OnSwapDestroyed(const bool reinit);

 public:
  explicit RenderPass(const std::string name, const VkRenderPassCreateInfo* create_info);
  ~RenderPass() override;

  auto GetDescriptors() const -> DescriptorSet* {
    return descriptors_;
  }

  auto GetPipeline() const -> RenderPipeline* {
    return pipeline_;
  }

  auto GetNext() const -> RenderPass* {
    return next_;
  }

  inline auto HasNext() const -> bool {
    return GetNext() != nullptr;
  }

  void SetNext(RenderPass* rhs) {
    ASSERT(rhs);
    next_ = rhs;
  }

  auto ToString() const -> std::string override;

  operator VkRenderPass() const {
    return GetHandle();
  }
};

using RenderPassIterator = SinglyLinkedListIteratorTemplate<RenderPass>;

auto VisitAllRenderPasses(RenderPassVisitor* vis) -> bool;
auto VisitAllRenderPasses(RenderPassPredicate vis) -> bool;

auto FindRenderPass(const std::string name) -> RenderPass*;

template <class Target, const uint64_t NumberOfTargets = MAX_NUMBER_OF_FRAMES_IN_FLIGHT + 1>
class RenderPassTemplate : public RenderPass {
 protected:
  std::array<Target*, NumberOfTargets> targets_{};

  RenderPassTemplate(const std::string name, const VkRenderPassCreateInfo* create_info) :
    RenderPass(std::move(name), create_info) {}

  inline auto GetTargetFramebuffer(const uint32_t idx) const -> vk::Framebuffer* {
    return GetTarget(idx)->GetFramebuffer();
  }

 public:
  ~RenderPassTemplate() override = default;

  auto GetTarget(const uint64_t idx) const -> Target* {
    return targets_.at(idx);
  }
};

class RenderPassScope {
 private:
  RenderPass* pass_;
  VkRenderPassBeginInfo begin_info_{};
  VkCommandBuffer buffer_;

 public:
  explicit RenderPassScope(VkCommandBuffer buffer, RenderPass* pass, const VkFramebuffer& framebuffer,
                           const std::vector<VkClearValue>& clear_values = {});
  ~RenderPassScope();

  auto GetRenderPass() const -> RenderPass* {
    return pass_;
  }

  auto GetBeginInfo() const -> VkRenderPassBeginInfo const& {
    return begin_info_;
  }

  void Bind(RenderPipeline* pipeline);

  operator RenderPass*() const {
    return GetRenderPass();
  }
};

class RenderPassFinalizer : public RenderPassVisitor, public BaseFinalizer {
 public:
  RenderPassFinalizer() = default;
  ~RenderPassFinalizer() override = default;
  auto Visit(RenderPass* rhs) -> bool override;

 public:
  static void FinalizeAll();
  static void FinalizeAll(const std::vector<RenderPass*>& rhs);
};
}  // namespace vk
}  // namespace prt

#endif  // PRT_RENDER_PASS_H
