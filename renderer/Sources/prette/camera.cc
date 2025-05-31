#include "prette/camera.h"

#include <string>
#include <vector>

#include "prette/assertions.h"
#include "prette/camera_manager.h"
#include "prette/common.h"
#include "prette/copy_to_buffer.h"
#include "prette/gfx.h"
#include "prette/glm.h"
#include "prette/keyboard/keyboard.h"
#include "prette/mouse/mouse.h"
#include "prette/mouse/mouse_event.h"
#include "prette/platform.h"
#include "prette/rx.h"
#include "prette/swapchain/swapchain.h"
#include "prette/swapchain/swapchain_event.h"
#include "prette/thread_local.h"
#include "prette/tile.h"
#include "prette/to_string.h"
#include "prette/vk.h"
#include "prette/vk_buffer.h"

namespace prt {
static ThreadLocal<Camera> camera_{};

Camera::Camera(const glm::vec2 viewport_size, const glm::vec3 pos, const glm::vec3 dir) :
  data_(),
  buffer_(buffer_ = vk::Buffer::New(sizeof(CameraData),
                                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)) {
  ASSERT_INITIALIZED(buffer_);
  data_.viewport_size = viewport_size;
  data_.direction = dir;
  data_.pos = pos;
  UpdateProjectionMatrix();
  UpdateViewMatrix();
  on_key_ = OnKeyPressed().subscribe([this](KeyStateEvent* event) {
    if (event->IsCode(GLFW_KEY_UP)) {
      IncrementZoom();
    } else if (event->IsCode(GLFW_KEY_DOWN)) {
      DecrementZoom();
    } else if (event->IsCode(GLFW_KEY_A)) {
      MoveLeft();
    } else if (event->IsCode(GLFW_KEY_D)) {
      MoveRight();
    } else if (event->IsCode(GLFW_KEY_W)) {
      MoveUp();
    } else if (event->IsCode(GLFW_KEY_S)) {
      MoveDown();
    }
  });
  on_swap_init_ = OnSwapchainInit([this](SwapchainInitEvent* event) {
    UpdateProjectionMatrix();
    UpdateViewMatrix();
  });
  on_drag_start_ = OnDragStartEvent().subscribe([this](DragStartEvent* event) {
    dragging_ = OnMouseMotionEvent().subscribe([this](MouseMotionEvent* event) {
      if (event->IsUp()) {
        MoveUp(abs(event->GetDirection().y) / 100.0f);
      } else if (event->IsDown()) {
        MoveDown(abs(event->GetDirection().y) / 100.0f);
      } else if (event->IsLeft()) {
        MoveLeft(abs(event->GetDirection().x) / 100.0f);
      } else if (event->IsRight()) {
        MoveRight(abs(event->GetDirection().x) / 100.0f);
      }
    });
  });
  on_drag_finish_ = OnDragFinishedEvent().subscribe([this](DragFinishedEvent* event) {
    dragging_.unsubscribe();
  });
  on_scroll_ = OnScrollEvent().subscribe([this](ScrollEvent* event) {
    const auto deltaY = event->GetDelta().y;
    if (event->IsScrollUp()) {
      IncrementZoom();
    } else if (event->IsScrollDown()) {
      DecrementZoom();
    }
  });
  GetCameraManager()->Register(this);
}

Camera::~Camera() {
  GetCameraManager()->Deregister(this);
  on_key_.unsubscribe();
  dragging_.unsubscribe();
  on_scroll_.unsubscribe();
  delete buffer_;
}

auto Camera::ToString() const -> std::string {
  return ToStringHelper<Camera>{};
}

void Camera::UpdateViewMatrix() {
  data_.view = glm::lookAt(GetPos(), GetPos() + GetDirection(), GetUp());
}

void Camera::Update() {
  UpdateViewMatrix();
  ASSERT_INITIALIZED(GetBuffer());
  vk::CopyBytesToBufferWithStaging::Copy(data_, GetBuffer());
}

auto Camera::Unproject(const glm::vec2 ndc) const -> glm::vec3 {
  glm::vec4 clip_coords = glm::vec4(ndc.x, ndc.y, -1.0f, 1.0f);
  glm::mat4 pv = GetProjection() * GetView();
  glm::mat4 ipv = glm::inverse(pv);
  glm::vec4 world_coords = ipv * clip_coords;
  return world_coords;
}

void Camera::UpdateProjectionMatrix() {
  const auto ar = GetViewportAspectRatio();
  const auto w = (GetViewportWidth() / kTileSizeInPixels) * 0.5f;
  const auto h = (GetViewportHeight() / kTileSizeInPixels) * 0.5f * ar;
  const auto z = GetZoomPercent() + 0.2f;
  data_.projection = glm::ortho(-w * z, w * z, -h * z, h * z, kNearClip, kFarClip);
}

void Camera::SetPos(const glm::vec3& pos) {
  data_.pos = pos;
  UpdateViewMatrix();
}

void Camera::SetZoom(const float rhs) {
  data_.zoom = rhs;
  Clamp(data_.zoom, kMinZoom, kMaxZoom);
  UpdateProjectionMatrix();
}

auto CameraFinalizer::Visit(Camera* rhs) -> bool {
  ASSERT(rhs);
  DVLOG(2) << "finalizing " << rhs->ToString();
  delete rhs;
  num_finalized_++;
  return true;
}

void CameraFinalizer::FinalizeAll(const std::vector<Camera*>& all) {
  DVLOG(1) << "finalizing Cameras....";
  CameraFinalizer finalizer{};
  for (const auto& cam : all) {
    LOG_IF(FATAL, !finalizer.Visit(cam)) << "failed to finalize: " << cam->ToString();
  }
  DVLOG(1) << "finalized " << finalizer.GetNumberOfObjectsFinalized() << " Cameras";
}
}  // namespace prt