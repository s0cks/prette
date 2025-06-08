#include "prette/gui/gui_system.h"

#include <cstdint>
#include <vector>
#include <yoga/YGConfig.h>
#include <yoga/YGEnums.h>
#include <yoga/YGNode.h>
#include <yoga/YGNodeLayout.h>
#include <yoga/YGNodeStyle.h>
#include <yoga/YGValue.h>
#include <yoga/Yoga.h>

#include "prette/assertions.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/gfx.h"
#include "prette/gfx_driver_event.h"
#include "prette/glm.h"
#include "prette/gui/gui.h"
#include "prette/gui/gui_debug.h"
#include "prette/gui/gui_render_pass.h"
#include "prette/renderer.h"
#include "prette/renderer_event.h"
#include "prette/swapchain/swapchain.h"
#include "prette/swapchain/swapchain_event.h"
#include "prette/thread_local.h"
#include "prette/window/window.h"

namespace prt {
static ThreadLocal<GuiSystem> system_{};

static inline void InitStyle() {
  ImGui::StyleColorsDark();
}

static inline void InitImGuiContext() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  const auto window = GetAppWindow();
  ASSERT(window);
  ImGui_ImplGlfw_InitForVulkan(window->GetHandle(), true);
  auto& io = ImGui::GetIO();
  io.IniFilename = nullptr;
  InitStyle();
}

GuiSystem::GuiSystem() {
  InitImGuiContext();
  on_pre_frame_ = OnPreFrame([this](PreFrameEvent* event) {
    UpdateImGui();
  });
  on_post_init_ = OnPostInit([this](PostInitEvent* event) {
    guis_.push_back(new GuiDebug());
  });
  on_swap_created_ = OnSwapchainCreated([this](SwapchainCreatedEvent* event) {
    pass_ = GuiRenderPass::New();
    ASSERT_INITIALIZED(pass_);
  });
  on_destroying_device_ = OnDestroyingDriver([this](DestroyingDriverEvent* event) {
    Shutdown();
  });
}

GuiSystem::~GuiSystem() {
  on_pre_frame_.unsubscribe();
}

struct GuiNode {
  uint64_t index;
  YGNodeRef node;
  Gui* gui;

  operator glm::vec4() const {
    return glm::vec4{
        YGNodeLayoutGetTop(node),
        YGNodeLayoutGetLeft(node),
        YGNodeLayoutGetWidth(node),
        YGNodeLayoutGetHeight(node),
    };
  }
};

static inline void CreateYogaTree(const YGNodeRef& root, const std::vector<Gui*>& guis, std::vector<GuiNode>& results) {
  results.resize(guis.size());
  for (auto idx = 0; idx < guis.size(); idx++) {
    results[idx] = {
        .index = idx,
        .node = guis[idx]->CreateYogaNode(),
        .gui = guis[idx],
    };
    YGNodeInsertChild(root, results[idx].node, idx);
  }
}

void GuiSystem::UpdateLayout() {
  std::vector<GuiNode> nodes{};
  const auto root = GetAppWindow()->CreateRootYogaNode();
  CreateYogaTree(root, guis_, nodes);
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
  for (const auto& node : nodes) {
    node.gui->SetBounds((glm::vec4)node);
  }

  YGNodeFreeRecursive(root);
  ClearLayoutsChanged();
}

void GuiSystem::Shutdown() {
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
}

void GuiSystem::UpdateImGui() {
  const auto window = GetAppWindow();
  ASSERT(window);
  const auto size = window->GetSize();

  auto& io = ImGui::GetIO();
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  if (HaveLayoutsChanged())
    UpdateLayout();

  for (const auto& gui : guis_) {
    gui->Update();
    gui->Render();
  }

  ImGui::Render();
}

void GuiSystem::InitSystem() {
  ASSERT(!IsSystemInitialized());
  system_ = new GuiSystem();
}

auto GuiSystem::GetSystem() -> GuiSystem* {
  ASSERT(IsSystemInitialized());
  return system_;
}

auto GuiSystem::IsSystemInitialized() -> bool {
  return system_.Get() != nullptr;
}
}  // namespace prt