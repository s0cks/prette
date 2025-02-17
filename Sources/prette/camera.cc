#include "prette/camera.h"

#include "prette/common.h"
#include "prette/mouse.h"
#include "prette/swapchain.h"
#include "prette/window.h"

namespace prt {
PerspectiveCamera::PerspectiveCamera(const float fov, const float aspectRatio, const float nearClip, const float farClip,
                                     const glm::vec3& pos) :
  Camera(glm::perspective(fov, aspectRatio, nearClip, farClip), pos) {}

PerspectiveCamera::~PerspectiveCamera() {
  on_mouse_moved_.unsubscribe();
}

void PerspectiveCamera::Update() {
  glm::vec3 front;
  front.x = cos(glm::radians(GetYaw()) * cos(glm::radians(GetPitch())));
  front.y = sin(glm::radians(GetPitch()));
  front.z = sin(glm::radians(GetYaw()) * cos(glm::radians(GetPitch())));
  data_.direction = glm::normalize(front);
  data_.right = glm::normalize(glm::cross(GetDirection(), GetUp()));
  data_.up = glm::normalize(glm::cross(GetRight(), GetDirection()));
  UpdateViewMatrix();
}

static Camera* camera_ = nullptr;

void Camera::Init() {
  OnSwapChainInitEvent().subscribe([](SwapChainInitEvent* event) {
    ASSERT(event);
    const auto window = GetAppWindow();
    ASSERT(window);
    const auto fbsize = window->GetFramebufferSize();
    const auto aspectRatio = (fbsize.width() / fbsize.height());
    camera_ = new PerspectiveCamera(70.0f, aspectRatio, 0.0f, 100.0f, glm::vec3(0.0f, 0.0f, 3.0f));
    ASSERT(camera_);
  });
}

auto Camera::Get() -> Camera* {
  ASSERT(camera_);
  return camera_;
}
}  // namespace prt