#include "prette/gui_viewport.h"

#include "prette/common.h"
#include "prette/glm.h"
#include "prette/gui.h"
#include "prette/gui_renderer.h"
#include "prette/thread_local.h"
#include "prette/window.h"
#include "prette/window_event.h"

// TODO: move
namespace glm {
static inline auto to_imvec2(const glm::vec2& rhs) -> ImVec2 {
  return ImVec2{
      rhs.x,
      rhs.y,
  };
}
}  // namespace glm

namespace prt {
GuiViewport::GuiViewport() :
  Gui("Viewport") {
  LoadLuaScript("gui/viewport.lua");
  InvokeLuaCallback("init");
  const auto window = GetAppWindow();
  ASSERT(window);
  const auto win_size = window->GetSize();
  pos_ = glm::vec2(win_size.width() / 4, 0);
  size_ = glm::vec2(win_size.width() - (win_size.width() / 4), win_size.height());
  OnWindowSizeEvent().subscribe([this](WindowSizeEvent* event) {
    const auto window = GetAppWindow();
    ASSERT(window);
    const auto win_size = window->GetSize();
    pos_ = glm::vec2(win_size.width() / 4, 0);
    size_ = glm::vec2(win_size.width() - (win_size.width() / 4), win_size.height());
  });
}

void GuiViewport::Update() {
  InvokeLuaCallback("update");
}

void GuiViewport::Render() {
  ImGui::SetNextWindowPos(glm::to_imvec2(pos_));
  ImGui::SetNextWindowSize(glm::to_imvec2(size_));
  ImGui::Begin(GetGuiName(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration);
  ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
  ImGui::Image(GetGuiRenderer()->GetCurrentSceneDescriptor(), ImVec2{viewportPanelSize.x, viewportPanelSize.y});
  ImGui::End();
}

static ThreadLocal<GuiViewport> instance_{};

auto GuiViewport::IsInitialized() -> bool {
  return instance_.Get() != nullptr;
}

auto GuiViewport::Init() -> GuiViewport* {
  ASSERT(!IsInitialized());
  const auto instance = new GuiViewport();
  instance_ = instance;
  ASSERT(IsInitialized());
  return instance;
}

auto GuiViewport::Get() -> GuiViewport* {
  ASSERT(IsInitialized());
  return instance_.Get();
}
}  // namespace prt