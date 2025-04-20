#include "prette/camera.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

#include "prette/common.h"
#include "prette/engine.h"
#include "prette/glm.h"
#include "prette/keyboard.h"
#include "prette/mouse.h"
#include "prette/settings.h"
#include "prette/swapchain.h"
#include "prette/window.h"

namespace prt {
OrthoCamera::OrthoCamera(const float left, const float right, const float bottom, const float top, const glm::vec3& pos) :
  Camera(kOrthoCamera, glm::ortho(left, right, bottom, top, kDefaultNearClip, kDefaultFarClip), pos) {
  on_key_ = OnKeyStateEvent().subscribe([this](KeyStateEvent* event) {
    ASSERT(event);
  });
  UpdateViewMatrix();
}

OrthoCamera::~OrthoCamera() {
  on_key_.unsubscribe();
}

void OrthoCamera::UpdateViewMatrix() {
  Camera::UpdateViewMatrix();
}

void OrthoCamera::Update() {
  UpdateViewMatrix();
}

PerspectiveCamera::PerspectiveCamera(const float fov, const float aspectRatio, const float nearClip, const float farClip,
                                     const glm::vec3& pos) :
  fov_(fov),
  aspect_(aspectRatio),
  near_(nearClip),
  far_(farClip),
  Camera(kPerspectiveCamera, glm::perspective(fov, aspectRatio, nearClip, farClip), pos) {
  on_key_ = OnKeyStateEvent().subscribe([this](KeyStateEvent* event) {
    if (event->IsRepeat() || event->IsPressed()) {
      ASSERT(event);
      const auto engine = Engine::Get();
      ASSERT(engine);
      const auto velocity = CalculateVelocity(engine->GetCurrentTick() - engine->GetPreviousTick());
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
    const auto fb_size = window->GetFramebufferSize();
    const auto aspect_ratio = fb_size.GetAspectRatio();
    // camera_ = new PerspectiveCamera(kDefaultFov, aspect_ratio, kDefaultNearClip, kDefaultFarClip, kDefaultPos);
    camera_ = new OrthoCamera(-aspect_ratio, aspect_ratio, -1.0f, 1.0f, glm::vec3(0.0f, 0.0f, 3.0f));
    //    camera_ = new OrthoCamera(0, fb_size.width() / 32, 0, fb_size.height() / 32, glm::vec3(0.0f, 0.0f, 3.0f));
    ASSERT(camera_);
  });
}

auto Camera::Get() -> Camera* {
  ASSERT(camera_);
  return camera_;
}
}  // namespace prt