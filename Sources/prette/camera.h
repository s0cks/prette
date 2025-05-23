#ifndef PRT_CAMERA_H
#define PRT_CAMERA_H

#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

#include "prette/common.h"
#include "prette/glm.h"
#include "prette/rx.h"
#include "prette/vk_buffer.h"

namespace prt {
static constexpr const auto kWorldUp = glm::vec3(0.0f, -1.0f, 0.0f);
static constexpr const auto kWorldDown = glm::vec3(0.0f, 1.0f, 0.0f);
static constexpr const auto kWorldRight = glm::vec3(1.0f, 0.0f, 0.0f);
static constexpr const auto kWorldLeft = glm::vec3(-1.0f, 0.0f, 0.0f);

static constexpr const auto kDefaultZoom = 100.0f;
static constexpr const auto kZoomInterval = 2.0f;
static constexpr const auto kMinZoom = 1.0f;
static constexpr const auto kMaxZoom = 100.0f;

static constexpr const auto kNearClip = 0.1f;
static constexpr const auto kFarClip = 1000.0f;

static constexpr const auto kDefaultCameraSpeed = 1.0f;

struct CameraData {
  alignas(8) glm::vec2 viewport_size{};
  alignas(16) glm::mat4 projection{1.0f};
  alignas(16) glm::mat4 view{1.0f};
  alignas(16) glm::vec3 pos{};
  alignas(16) glm::vec3 up = kWorldUp;
  alignas(16) glm::vec3 right = kWorldRight;
  alignas(16) glm::vec3 direction{};
  alignas(4) float zoom = kDefaultZoom;
  alignas(4) float speed = kDefaultCameraSpeed;
};

#define FOR_EACH_CAMERA_DIRECTION(V) \
  V(Up)                              \
  V(Down)                            \
  V(Left)                            \
  V(Right)

class Camera;
using CameraList = std::vector<Camera*>;
using CameraSet = std::unordered_set<Camera*>;
using CameraPredicate = std::function<bool(Camera*)>;

class CameraVisitor {
 protected:
  CameraVisitor() = default;

 public:
  virtual ~CameraVisitor() = default;
  virtual auto Visit(Camera* camera) -> bool = 0;
};

class Camera {
  friend class Renderer;
  friend class GuiCamera;
  friend class CameraManager;

 public:
  static constexpr const auto kDefaultDirection = glm::vec3(0.0f, 0.0f, 1.0f);
  static constexpr const auto kDefaultPos = glm::vec3(16.0f, 16.0f, 1.0f);

 protected:
  CameraData data_{};
  vk::Buffer* buffer_ = nullptr;
  rx::subscription on_key_{};
  rx::subscription on_scroll_{};
  rx::subscription dragging_{};
  rx::subscription on_drag_start_{};
  rx::subscription on_drag_finish_{};
  rx::subscription on_swap_init_{};

  auto data() -> CameraData& {
    return data_;
  }

  auto pos() -> glm::vec3& {
    return data_.pos;
  }

  void UpdateViewMatrix();
  void UpdateProjectionMatrix();

 public:
  Camera(const glm::vec2 viewport_size, const glm::vec3 pos = kDefaultPos,
         const glm::vec3 direction = kDefaultDirection);
  virtual ~Camera();

  auto IsDragging() const -> bool {
    return dragging_.is_subscribed();
  }

  auto GetBuffer() const -> vk::Buffer* {
    return buffer_;
  }

  inline auto HasBuffer() const -> bool {
    return GetBuffer() != nullptr;
  }

  void SetBuffer(vk::Buffer* rhs) {
    ASSERT(rhs);
    buffer_ = rhs;
  }

  auto data() const -> const CameraData& {
    return data_;
  }

  auto GetViewportSize() const -> const glm::vec2& {
    return data().viewport_size;
  }

  auto GetViewportWidth() const -> float {
    return data().viewport_size[0];
  }

  auto GetViewportHeight() const -> float {
    return data().viewport_size[1];
  }

  auto GetViewportAspectRatio() const -> float {
    return GetViewportWidth() / GetViewportHeight();
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

  void SetPos(const glm::vec3& pos);

  auto GetZoom() const -> float {
    return data().zoom;
  }

  auto GetZoomPercent() const -> float {
    return (kMaxZoom - GetZoom()) / kMaxZoom;
  }

  void SetZoom(const float rhs);

  inline void IncrementZoom() {
    return SetZoom(GetZoom() + kZoomInterval);
  }

  inline void DecrementZoom() {
    return SetZoom(GetZoom() - kZoomInterval);
  }

  auto GetSpeed() const -> float {
    return data().speed;
  }

  void SetSpeed(const float rhs) {
    data().speed = rhs;
  }

  void Update();
  auto Unproject(const glm::vec2 pos) const -> glm::vec3;
  auto ToString() const -> std::string;

#define DEFINE_MOVE_DIRECTION(Name)                   \
  inline void Move##Name(const float amount = 1.0f) { \
    pos() += ((kWorld##Name * amount) * GetSpeed());  \
  }
  FOR_EACH_CAMERA_DIRECTION(DEFINE_MOVE_DIRECTION);
#undef DEFINE_MOVE_DIRECTION

 public:
  static auto Get() -> Camera*;
};

class CameraFinalizer : public CameraVisitor {
 private:
  uint64_t num_finalized_ = 0;

 public:
  CameraFinalizer() = default;
  ~CameraFinalizer() override = default;
  auto Visit(Camera* rhs) -> bool override;

  auto GetNumberOfObjectsFinalized() const -> uint64_t {
    return num_finalized_;
  }

 public:
  static void FinalizeAll(const std::vector<Camera*>& rhs);
};

auto GetCamera() -> Camera*;
}  // namespace prt

#endif  // PRT_CAMERA_H
