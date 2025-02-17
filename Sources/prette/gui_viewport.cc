#include "prette/gui_viewport.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <implot.h>

#include "prette/common.h"
#include "prette/gfx_driver.h"
#include "prette/gui_renderer.h"
#include "prette/lua.h"
#include "prette/swapchain.h"
#include "prette/window.h"

namespace prt {
GuiViewport::GuiViewport() :
  Gui("Viewport") {
  LoadLuaScript("gui/viewport.lua");
  InvokeLuaCallback("init");
}

void GuiViewport::Update() {
  InvokeLuaCallback("update");
}

void GuiViewport::Render() {
  const auto window = GetAppWindow();
  ASSERT(window);
  const auto size = window->GetSize();
  ImGui::SetNextWindowPos(ImVec2{(size.width() / 4), 0});
  ImGui::SetNextWindowSize(ImVec2{size.width() - (size.width() / 4), size.height()});
  ImGui::Begin(GetGuiName(), nullptr, ImGuiWindowFlags_NoCollapse);
  ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
  ImGui::Image(GuiRenderer::GetSceneDescriptor(SwapChain::GetCurrentFrame().frame),
               ImVec2{viewportPanelSize.x, viewportPanelSize.y});
  ImGui::End();
}

void GuiViewport::Init() {}
}  // namespace prt