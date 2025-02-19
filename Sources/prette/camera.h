#ifndef PRT_CAMERA_H
#define PRT_CAMERA_H

#include "prette/glm.h"
#include "prette/tick.h"

namespace prt {
static constexpr const auto kWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
static constexpr const auto kWorldRight = glm::vec3(0.0f, 0.0f, 1.0f);

struct CameraData {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 projection = glm::mat4(1.0f);
  alignas(16) glm::vec3 pos;
  alignas(16) glm::vec3 up;
  alignas(16) glm::vec3 right;
  alignas(16) glm::vec3 direction;
};

class Camera {
 protected:
  CameraData data_{};  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)

  Camera(const glm::mat4& projection, const glm::vec3& pos, const glm::vec3& direction = glm::vec3(0.0f, 0.0f, -1.0f)) :
    data_() {
    data_.direction = direction;
    data_.right = glm::normalize(glm::cross(kWorldUp, data_.direction));
    data_.up = glm::cross(data_.direction, data_.right);
    data_.projection = projection;
    data_.pos = pos;
    UpdateViewMatrix();
  }

  virtual void UpdateViewMatrix() {
    data_.view = glm::lookAt(GetPos(), GetPos() + GetDirection(), GetUp());
  }

 public:
  virtual ~Camera() = default;

  auto data() const -> const CameraData& {
    return data_;
  }

  auto GetView() const -> const glm::mat4& {
    return data().view;
  }

  auto GetProjection() const -> const glm::mat4& {
    return data().projection;
  }

  auto GetPos() const -> const glm::vec3& {
    return data().pos;
  }

  auto GetUp() const -> const glm::vec3& {
    return data().up;
  }

  auto GetRight() const -> const glm::vec3& {
    return data().right;
  }

  auto GetDirection() const -> const glm::vec3& {
    return data().direction;
  }

  void SetPos(const glm::vec3& pos) {
    data_.pos = pos;
    UpdateViewMatrix();
  }

  virtual void Update() {
    // do nothing?
  }

 public:
  static auto Get() -> Camera*;
  static void Init();
};

class PerspectiveCamera : public Camera {
 private:
  float yaw_ = -90.0f;
  float pitch_ = 0.0f;
  float speed_ = 0.05f;
  float sensitivity_ = 0.1f;
  rx::subscription on_mouse_moved_{};
  rx::subscription on_key_{};

  inline void MoveLeft(const float velocity) {
    data_.pos -= (data_.right * velocity);
  }

  inline void MoveBack(const float velocity) {
    data_.pos -= (data_.direction * velocity);
  }

  inline void MoveForward(const float velocity) {
    data_.pos += (data_.direction * velocity);
  }

  inline void MoveRight(const float velocity) {
    data_.pos += (data_.right * velocity);
  }

 public:
  PerspectiveCamera(const float fov, const float aspectRatio, const float nearClip, const float farClip, const glm::vec3& pos);
  ~PerspectiveCamera() override;

  auto GetYaw() const -> float {
    return yaw_;
  }

  auto GetPitch() const -> float {
    return pitch_;
  }

  auto GetSpeed() const -> float {
    return speed_;
  }

  void SetSpeed(const float rhs) {
    ASSERT(rhs >= 0.0f);
    speed_ = rhs;
  }

  auto GetSensitivity() const -> float {
    return sensitivity_;
  }

  void SetSensitivity(const float rhs) {
    ASSERT(rhs >= 0.0f);
    sensitivity_ = rhs;
  }

  void Update() override;
};
}  // namespace prt

#endif  // PRT_CAMERA_H
