#include "prette/render_pass/render_pass.h"

#include <functional>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/device.h"  // IWYU pragma: keep
#include "prette/gfx.h"
#include "prette/gfx_vk.h"
#include "prette/renderer.h"
#include "prette/swapchain/swapchain.h"
#include "prette/swapchain/swapchain_event.h"
#include "prette/to_string.h"
#include "prette/vk.h"
#include "prette/vk_cmd_buffers.h"

namespace prt::vk {
struct RenderPassHash {
  auto operator()(RenderPass* pass) const -> size_t {
    ASSERT(pass);
    size_t hash = 0;
    Combine(hash, pass->GetName());
    return hash;
  }

 private:
  template <class T>
  static inline void Combine(std::size_t& seed, const T& v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
};

struct RenderPassEq {
  auto operator()(RenderPass* lhs, RenderPass* rhs) const -> bool {
    return lhs->GetName() == rhs->GetName();
  }
};

static std::unordered_set<RenderPass*, RenderPassHash, RenderPassEq> render_passes_{};

static inline void Register(RenderPass* rhs) {
  ASSERT(rhs);
  const auto [_, success] = render_passes_.insert(rhs);
  LOG_IF(FATAL, !success) << "failed to register RenderPass: " << rhs->GetName();
}

static inline void Deregister(RenderPass* rhs) {
  ASSERT(rhs);
  const auto num_removed = render_passes_.erase(rhs);
  LOG_IF(FATAL, num_removed != 1) << "failed to deregister RenderPass named `" << rhs->GetName() << "`";
}

RenderPass::RenderPass(const std::string name, const VkRenderPassCreateInfo* create_info) :
  NamedHandleTemplate<VkRenderPass>(std::move(name)) {
  ASSERT(create_info && create_info->subpassCount > 0 && create_info->pSubpasses != nullptr);
  Register(this);
  const auto driver = Driver::Get();
  driver->CreateRenderPass(create_info, handle_ptr());
  if (IsSwapchainInitialized())
    command_buffers_ = CommandBufferPool<>::New();
  if (GetRenderer() != nullptr)
    GetRenderer()->AddRenderPass(this);  // TODO: remove this
  on_swap_init_ = OnSwapchainCreated([this](SwapchainCreatedEvent* event) {
    OnSwapInit(event->IsReinit());
  });
  on_swap_destroyed_ = OnSwapchainDestroyed([this](SwapchainDestroyedEvent* event) {
    OnSwapDestroyed(event->IsReinit());
  });
}

RenderPass::~RenderPass() {
  Driver::Get()->DestroyRenderPass(handle_ref());
  Deregister(this);
  delete command_buffers_;
  on_swap_init_.unsubscribe();
  on_swap_destroyed_.unsubscribe();
}

auto RenderPass::ToString() const -> std::string {
  ToStringHelper<RenderPass> helper{};
  helper.AddFieldRef("name", GetName());
  return helper;
}

auto VisitAllRenderPasses(RenderPassVisitor* vis) -> bool {
  for (const auto& pass : render_passes_) {
    if (!vis->Visit(pass))
      return false;
  }
  return true;
}

auto VisitAllRenderPasses(const std::function<bool(RenderPass*)>& vis) -> bool {
  for (const auto& pass : render_passes_) {
    if (!vis(pass))
      return false;
  }
  return true;
}

auto FindRenderPass(const std::string name) -> RenderPass* {
  for (const auto& pass : render_passes_) {
    if (EqualsIgnoreCase(pass->GetName(), name))
      return pass;
  }
  DLOG(WARNING) << "failed to find RenderPass w/ name " << name;
  return nullptr;
}

void RenderPass::SetPipeline(RenderPipeline* rhs) {
  ASSERT_INITIALIZED(rhs);
  pipeline_ = rhs;
}

void RenderPass::OnSwapInit(const bool reinit) {
  command_buffers_ = CommandBufferPool<>::New();
  UpdateDescriptors();
}

void RenderPass::OnSwapDestroyed(const bool reinit) {
  delete command_buffers_;
}

RenderPassScope::RenderPassScope(VkCommandBuffer buffer, RenderPass* pass, const VkFramebuffer& framebuffer,
                                 const std::vector<VkClearValue>& clear_values) :
  buffer_(buffer),
  pass_(pass) {
  begin_info_.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  begin_info_.renderPass = *GetRenderPass();
  begin_info_.framebuffer = framebuffer;
  begin_info_.renderArea = {
      .offset =
          {
              .x = 0,
              .y = 0,
          },
      .extent = GetSwapchain()->GetExtent(),
  };
  if (!clear_values.empty()) {
    begin_info_.clearValueCount = clear_values.size();
    begin_info_.pClearValues = clear_values.data();
  } else {
    begin_info_.pClearValues = nullptr;
    begin_info_.clearValueCount = 0;
  }
  vkCmdBeginRenderPass(buffer_, &begin_info_, VK_SUBPASS_CONTENTS_INLINE);
}

RenderPassScope::~RenderPassScope() {
  vkCmdEndRenderPass(buffer_);
}

void RenderPassScope::Bind(RenderPipeline* p) {
  ASSERT(p);
  vkCmdBindPipeline(buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, (*p));
}

auto RenderPassFinalizer::Visit(RenderPass* rhs) -> bool {
  ASSERT_INITIALIZED(rhs);
  DVLOG(2) << "finalizing: " << rhs->ToString();
  delete rhs;
  IncrementFinalizedCounter();
  return true;
}

void RenderPassFinalizer::FinalizeAll() {
  std::vector<RenderPass*> data(std::begin(render_passes_), std::end(render_passes_));
  return FinalizeAll(data);
}

void RenderPassFinalizer::FinalizeAll(const std::vector<RenderPass*>& rhs) {
  DVLOG(1) << "finalizing RenderPasses....";
  RenderPassFinalizer finalizer{};
  for (const auto& value : rhs) {
    LOG_IF(FATAL, !finalizer.Visit(value)) << "failed to finalize:  " << value->ToString();
  }
  DVLOG(1) << "finalized " << finalizer.GetNumberOfObjectsFinalized() << " RenderPasses.";
}
}  // namespace prt::vk