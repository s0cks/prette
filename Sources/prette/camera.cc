#include "prette/camera.h"

#include "prette/common.h"
#include "prette/config.h"
#include "prette/engine.h"
#include "prette/glm.h"
#include "prette/keyboard.h"
#include "prette/mouse.h"
#include "prette/swapchain.h"
#include "prette/window.h"

namespace prt {
PerspectiveCamera::PerspectiveCamera(const float fov, const float aspectRatio, const float nearClip, const float farClip,
                                     const glm::vec3& pos) :
  Camera(glm::perspective(fov, aspectRatio, nearClip, farClip), pos) {
  on_key_ = OnKeyStateEvent().subscribe([this](KeyStateEvent* event) {
    if (event->IsRepeat() || event->IsPressed()) {
      ASSERT(event);
      const auto engine = Engine::Get();
      ASSERT(engine);
      const auto dts = (engine->GetCurrentTick() - engine->GetPreviousTick());
      const auto velocity = speed_ * (dts.value() / NSEC_PER_MSEC);
      if (event->IsCode(GLFW_KEY_A)) {
        return MoveLeft(velocity);
      } else if (event->IsCode(GLFW_KEY_S)) {
        return MoveBack(velocity);
      } else if (event->IsCode(GLFW_KEY_W)) {
        return MoveForward(velocity);
      } else if (event->IsCode(GLFW_KEY_D)) {
        return MoveRight(velocity);
      }
    }
  });
  on_mouse_moved_ = OnMouseMotionEvent().skip(1).subscribe([this](MouseMotionEvent* event) {
    ASSERT(event);
    const auto keyboard = Keyboard::Get();
    ASSERT(keyboard);
    if (keyboard->GetKey(GLFW_KEY_SPACE).IsReleased()) {
      const auto& direction = event->GetDirection();
      yaw_ += (direction.x * sensitivity_);
      pitch_ -= (direction.y * sensitivity_);
      Clamp(pitch_, -89.0f, 89.0f);
      glm::vec3 dir{};
      dir.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
      dir.y = sin(glm::radians(pitch_));
      dir.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
      data_.direction = glm::normalize(dir);
    }
  });
}

PerspectiveCamera::~PerspectiveCamera() {
  on_mouse_moved_.unsubscribe();
  on_key_.unsubscribe();
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