#include "prette/camera_manager.h"

#include <iterator>

#include "prette/camera.h"
#include "prette/common.h"
#include "prette/glm.h"
#include "prette/renderer.h"
#include "prette/renderer_event.h"
#include "prette/swapchain.h"
#include "prette/swapchain_event.h"
#include "prette/thread_local.h"

namespace prt {
static ThreadLocal<Camera> camera_{};
static ThreadLocal<CameraManager> manager_{};

CameraManager::CameraManager() {
  on_swap_created_ = OnSwapchainCreatedEvent().subscribe([this](SwapchainCreatedEvent* event) {
    const auto camera = new Camera(glm::vec2(1920, 1080));
    ASSERT(camera);
    camera_ = camera;
  });
  on_pre_frame_ = OnPreFrameEvent().subscribe([this](PreFrameEvent* event) {
    UpdateAllCameras();
  });
}

CameraManager::~CameraManager() {
  on_swap_created_.unsubscribe();
  on_pre_frame_.unsubscribe();
}

void CameraManager::Register(Camera* rhs) {
  ASSERT(rhs);
  const auto [_, success] = cameras_.insert(rhs);
  LOG_IF(FATAL, !success) << "failed to register: " << rhs->ToString();
}

void CameraManager::Deregister(Camera* rhs) {
  ASSERT(rhs);
  const auto num_erased = cameras_.erase(rhs);
  LOG_IF(FATAL, num_erased != 1) << "failed to deregister: " << rhs->ToString();
}

void CameraManager::UpdateAllCameras() {
  DVLOG(1) << "updating " << GetNumberOfRegisteredCameras() << " Cameras....";
  for (const auto& camera : cameras_) {
    DVLOG(2) << "updating: " << camera->ToString() << "....";
    camera_->Update();
  }
}

void CameraManager::FinalizeAllCameras() {
  CameraList cameras(std::begin(cameras_), std::end(cameras_));
  return CameraFinalizer::FinalizeAll(cameras);
}

auto CameraManager::VisitAllCameras(CameraPredicate vis) const -> bool {
  for (const auto& camera : cameras_) {
    if (!vis(camera))
      return false;
  }
  return true;
}

auto CameraManager::VisitAllCameras(CameraVisitor* vis) const -> bool {
  for (const auto& camera : cameras_) {
    if (!vis->Visit(camera))
      return false;
  }
  return true;
}

auto HasCamera() -> bool {
  return camera_.Get() != nullptr;
}

auto GetCamera() -> Camera* {
  ASSERT(HasCamera());
  return camera_;
}

void InitCameraManager() {
  ASSERT(!IsCameraManagerInitiailized());
  manager_ = new CameraManager();
  ASSERT(IsCameraManagerInitiailized());
}

auto GetCameraManager() -> CameraManager* {
  ASSERT(IsCameraManagerInitiailized());
  return manager_;
}

auto IsCameraManagerInitiailized() -> bool {
  return manager_.Get() != nullptr;
}
}  // namespace prt