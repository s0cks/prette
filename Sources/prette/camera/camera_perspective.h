#ifndef PRT_CAMERA_PERSPECTIVE_H
#define PRT_CAMERA_PERSPECTIVE_H

#include <ostream>

#include "prette/rx.h"
#include "prette/uri.h"
#include "prette/uv/utils.h"
#include "prette/mouse/mouse.h"
#include "prette/camera/camera.h"
#include "prette/keyboard/keyboard.h"

namespace prt::camera {
  struct PerspectiveCameraData {
    glm::vec4 viewport;
    glm::vec3 up;
    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 right;
    glm::mat4 projection;
    glm::mat4 view;
    float yaw;
    float pitch;
    float speed;
    float sensitivity;
    float zoom;

    PerspectiveCameraData();
    PerspectiveCameraData(const glm::vec2& viewport_size,
                          const glm::vec3& f,
                          const glm::vec3& p,
                          const glm::vec3& u);
    PerspectiveCameraData(const PerspectiveCameraData& rhs) = default;
    ~PerspectiveCameraData() = default;

    friend std::ostream& operator<<(std::ostream& stream, const PerspectiveCameraData& rhs) {
      stream << "PerspectiveCameraData(";
      stream << ")";
      return stream;
    }

    friend class PerspectiveCamera;
  private:
    float CalculateVelocity(const uint64_t dts) const {
      return (speed * 100.0f) * ((1.0f * dts) / NSEC_PER_SEC);
    }

    glm::mat4 CalculateView() const;
    glm::mat4 CalculateProjection() const;
  };

  class PerspectiveCamera : public Camera {
  public:
    static constexpr const auto kDefaultFront = glm::vec3(0.0f, 0.0f, -1.0f);
    static constexpr const auto kDefaultPos = glm::vec3(0.0f);
    static constexpr const auto kDefaultUp = glm::vec3(0.0f, 1.0f, 0.0f);
  protected:
    PerspectiveCameraData data_;
    rx::subscription on_key_pressed_;
    rx::subscription on_mouse_moved_;

    void Update();
    void OnKeyPressed(const keyboard::KeyCode code);
    void OnMouseMoved(const mouse::MouseMoveEvent* event);

    void UpdateUbo();
  public:
    explicit PerspectiveCamera(const Metadata& meta, const PerspectiveCameraData& data);
    ~PerspectiveCamera() override;
    std::string ToString() const override;

    const PerspectiveCameraData& GetData() const {
      return data_;
    }

    float GetYaw() const {
      return data_.yaw;
    }

    float GetPitch() const {
      return data_.pitch;
    }

    float GetSpeed() const {
      return data_.speed;
    }

    float GetSensitivity() const {
      return data_.sensitivity;
    }

    float GetZoom() const {
      return data_.zoom;
    }

    const glm::mat4& GetProjection() const override {
      return data_.projection;
    }

    const glm::mat4& GetView() const override {
      return data_.view;
    }
  public:
    static inline PerspectiveCamera* New(const Metadata& meta, const PerspectiveCameraData& data) {
      return new PerspectiveCamera(meta, data);
    }

    static inline PerspectiveCamera* New(const PerspectiveCameraData& data) {
      return New(Metadata(), data);
    }

    static PerspectiveCamera* FromJson(const uri::Uri& uri);
    static PerspectiveCamera* FromJson(const std::string& value);
  };

  PerspectiveCamera* GetPerspectiveCamera();
}

#endif //PRT_CAMERA_PERSPECTIVE_H