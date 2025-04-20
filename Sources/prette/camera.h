#ifndef PRT_CAMERA_H
#define PRT_CAMERA_H

#include "prette/glm.h"
#include "prette/tick.h"

namespace prt {
static constexpr const auto kWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
static constexpr const auto kWorldRight = glm::vec3(0.0f, 0.0f, 1.0f);

struct CameraData {
  alignas(16) glm::mat4 model{1.0f};
  alignas(16) glm::mat4 view{1.0f};
  alignas(16) glm::mat4 projection = glm::mat4(1.0f);
  alignas(16) glm::vec3 pos{};
  alignas(16) glm::vec3 up{};
  alignas(16) glm::vec3 right{};
  alignas(16) glm::vec3 direction{};
};

enum CameraType {
  kOrthoCamera,
  kIsoCamera,
  kPerspectiveCamera,
};

class Camera {
 protected:
  CameraType type_;
  CameraData data_{};

  Camera(const CameraType type, const glm::mat4& projection, const glm::vec3& pos,
         const glm::vec3& direction = glm::vec3(0.0f, 0.0f, -1.0f)) :
    type_(type),
    data_() {
    data_.direction = direction;
    data_.right = glm::normalize(glm::cross(kWorldUp, data_.direction));
    data_.up = glm::cross(data_.direction, data_.right);
    data_.projection = projection;
    data_.pos = pos;
  }

  virtual void UpdateViewMatrix() {
    data_.view = glm::lookAt(GetPos(), GetPos() + GetDirection(), GetUp());
  }

 public:
  virtual ~Camera() = default;

  auto GetType() const -> CameraType {
    return type_;
  }

  inline auto IsOrtho() const -> bool {
    return GetType() == kOrthoCamera;
  }

  inline auto IsIso() const -> bool {
    return GetType() == kIsoCamera;
  }

  inline auto IsPerspective() const -> bool {
    return GetType() == kPerspectiveCamera;
  }

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

class OrthoCamera : public Camera {
 private:
  rx::subscription on_key_{};

 protected:
  void UpdateViewMatrix() override;

 public:
  OrthoCamera(const float top, const float left, const float bottom, const float right, const glm::vec3& pos);
  ~OrthoCamera() override;

  void Update() override;
};

static constexpr const auto kDefaultYaw = -90.0f;
static constexpr const auto kDefaultSpeed = 0.05f;
static constexpr const auto kDefaultNearClip = 0.1f;
static constexpr const auto kDefaultFarClip = 1000.0f;
static constexpr const auto kDefaultPos = glm::vec3(0.0f, 0.0f, 3.0f);
static constexpr const auto kDefaultFov = 70.0f;
static constexpr const auto kDefaultSensitivity = 0.1f;
class PerspectiveCamera : public Camera {
 private:
  float yaw_ = kDefaultYaw;
  float pitch_{};
  float speed_ = kDefaultSpeed;
  float sensitivity_ = kDefaultSensitivity;
  float fov_;
  float aspect_;
  float near_;
  float far_;
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

  inline auto CalculateVelocity(const TickDelta& dts) -> float {
    return speed_ * (dts / NSEC_PER_MSEC);
  }

 public:
  PerspectiveCamera(const float fov, const float aspectRatio, const float nearClip, const float farClip, const glm::vec3& pos);
  ~PerspectiveCamera() override;

  auto GetFov() const -> float {
    return fov_;
  }

  void SetFov(const float rhs) {
    fov_ = rhs;
  }

  auto GetAspectRatio() const -> float {
    return aspect_;
  }

  void SetAspectRatio(const float rhs) {
    aspect_ = rhs;
  }

  auto GetNearClip() const -> float {
    return near_;
  }

  void SetNearClip(const float rhs) {
    near_ = rhs;
  }

  auto GetFarClip() const -> float {
    return far_;
  }

  void SetFarClip(const float rhs) {
    far_ = rhs;
  }

  auto GetYaw() const -> float {
    return yaw_;
  }

  void SetYaw(const float rhs) {
    yaw_ = rhs;
  }

  auto GetPitch() const -> float {
    return pitch_;
  }

  void SetPitch(const float rhs) {
    pitch_ = rhs;
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

  void SetDirection(const glm::vec3& rhs) {
    data_.direction = rhs;
    UpdateViewMatrix();
  }

  void Update() override;
};
}  // namespace prt

#endif  // PRT_CAMERA_H
