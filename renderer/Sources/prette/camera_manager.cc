#include "prette/camera_manager.h"

#include <iterator>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/camera.h"
#include "prette/common.h"
#include "prette/descriptor_set_builder.h"
#include "prette/descriptor_set_update.h"
#include "prette/glm.h"
#include "prette/renderer.h"
#include "prette/renderer_event.h"
#include "prette/swapchain/swapchain.h"
#include "prette/swapchain/swapchain_event.h"
#include "prette/thread_local.h"

namespace prt {
static ThreadLocal<Camera> camera_{};
static ThreadLocal<CameraManager> manager_{};

CameraManager::CameraManager() {
  OnInitDescriptorSets([this](InitDescriptorSetsEvent* event) {
    {
      vk::DescriptorSetBuilder builder{};
      builder.WithName("camera");
      builder.AddUniformBufferBinding(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);  // camera
      descriptors_ = builder;
      ASSERT_INITIALIZED(descriptors_);
    }

    {
      vk::DescriptorSetUpdate update(GetDescriptorSet());
      update.AddWriteCameraUniformBuffer(0, camera_);
    }
  });
  on_swap_created_ = OnSwapchainCreated([this](SwapchainCreatedEvent* event) {
    const auto camera = new Camera(glm::vec2(1920, 1080));
    ASSERT(camera);
    camera_ = camera;
  });
  on_pre_frame_ = OnPreFrame([this](PreFrameEvent* event) {
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
  DVLOG(2) << "updating " << GetNumberOfRegisteredCameras() << " Cameras....";
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