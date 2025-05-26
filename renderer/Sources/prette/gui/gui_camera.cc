#include "prette/gui/gui_camera.h"

#include "prette/assertions.h"
#include "prette/camera.h"
#include "prette/gui/gui.h"
#include "prette/mouse/mouse.h"
#include "prette/window/window.h"

namespace prt {
GuiCamera::GuiCamera() :
  Gui("Camera") {
  Update();
}

void GuiCamera::Update() {}

void GuiCamera::Render() {
  const auto window = GetAppWindow();
  ASSERT(window);
  Update();
  const auto size = window->GetSize();
  ImGui::SetNextWindowPos(ImVec2{
      bounds_.x,
      bounds_.y,
  });
  ImGui::SetNextWindowSize(ImVec2{
      bounds_.z,
      bounds_.w,
  });
  ImGui::Begin(GetGuiName(), nullptr, ImGuiWindowFlags_NoCollapse);
  const auto camera = GetCamera();
  ASSERT(camera);
  auto& data = camera->data();
  auto& pos = data.pos;
  if (ImGui::InputFloat("x", &pos.x)) {
    camera->SetPos(pos);
  }
  if (ImGui::InputFloat("y", &pos.y)) {
    camera->SetPos(pos);
  }
  if (ImGui::InputFloat("z", &pos.z)) {
    camera->SetPos(pos);
  }
  ImGui::NewLine();
  if (ImGui::InputFloat("Zoom", &data.zoom)) {
    camera->SetZoom(data.zoom);
  }
  if (ImGui::InputFloat("Speed", &data.speed)) {
    camera->SetZoom(data.speed);
  }

  ImGui::NewLine();
  auto world_pos = Mouse::Get()->GetWorldPos();
  ImGui::Text("World");  // NOLINT(cppcoreguidelines-pro-type-vararg)
  if (ImGui::InputFloat("x", &world_pos.x)) {}
  if (ImGui::InputFloat("y", &world_pos.y)) {}
  ImGui::End();
}

void GuiCamera::Init() {}
}  // namespace prt