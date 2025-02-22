#include "prette/gui_camera.h"

#include <imgui.h>

#include "prette/camera.h"
#include "prette/window.h"

namespace prt {
static float speed_ = 0.0f;
static float fov_ = 0.0f;
static float sensitivity_ = 0.0f;
static glm::vec3 pos_ = glm::vec3(0.0f);
static float yaw_ = 0.0f;
static float pitch_ = 0.0f;
static float aspect_ = 0.0f;
static float near_ = 0.0f;
static float far_ = 0.0f;

GuiCamera::GuiCamera() :
  Gui("Camera") {
  Update();
}

void GuiCamera::Update() {
  const auto camera = Camera::Get();
  ASSERT(camera);
  pos_ = ((PerspectiveCamera*)camera)->GetPos();
  sensitivity_ = ((PerspectiveCamera*)camera)->GetSensitivity();
  speed_ = ((PerspectiveCamera*)camera)->GetSpeed();
  yaw_ = ((PerspectiveCamera*)camera)->GetYaw();
  pitch_ = ((PerspectiveCamera*)camera)->GetPitch();
  fov_ = ((PerspectiveCamera*)camera)->GetFov();
  aspect_ = ((PerspectiveCamera*)camera)->GetAspectRatio();
  near_ = ((PerspectiveCamera*)camera)->GetNearClip();
  far_ = ((PerspectiveCamera*)camera)->GetFarClip();
}

void GuiCamera::Render() {
  const auto window = GetAppWindow();
  ASSERT(window);
  const auto size = window->GetSize();
  ImGui::SetNextWindowPos(ImVec2{0, size.height() / 2});
  ImGui::SetNextWindowSize(ImVec2{size.width() / 4, size.height() / 2});
  ImGui::Begin(GetGuiName(), nullptr, ImGuiWindowFlags_NoCollapse);
  const auto& camera = ((PerspectiveCamera*)Camera::Get());
  Update();
  if (ImGui::InputFloat("x", &pos_.x)) {
    camera->SetPos(pos_);
  }
  if (ImGui::InputFloat("y", &pos_.y)) {
    camera->SetPos(pos_);
  }
  if (ImGui::InputFloat("z", &pos_.z)) {
    camera->SetPos(pos_);
  }
  ImGui::NewLine();
  if (ImGui::InputFloat("Speed", &speed_)) {
    camera->SetSpeed(speed_);
  }
  if (ImGui::InputFloat("Sensitivity", &sensitivity_)) {
    camera->SetSensitivity(sensitivity_);
  }
  if (ImGui::InputFloat("Yaw", &yaw_)) {
    camera->SetYaw(yaw_);
  }
  if (ImGui::InputFloat("Pitch", &pitch_)) {
    camera->SetPitch(pitch_);
  }
  if (ImGui::InputFloat("Fov", &fov_)) {
    camera->SetFov(fov_);
  }
  if (ImGui::InputFloat("Aspect Ratio", &aspect_)) {
    camera->SetAspectRatio(aspect_);
  }
  if (ImGui::InputFloat("Near Clip", &near_)) {
    camera->SetNearClip(near_);
  }
  if (ImGui::InputFloat("Far Clip", &far_)) {
    camera->SetFarClip(far_);
  }
  ImGui::End();
}

void GuiCamera::Init() {}
}  // namespace prt