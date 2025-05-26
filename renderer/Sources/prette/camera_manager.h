#ifndef PRT_CAMERA_MANAGER_H
#define PRT_CAMERA_MANAGER_H

#include "prette/camera.h"
#include "prette/common.h"
#include "prette/rx.h"

namespace prt {
class CameraManager {
  friend class Camera;
  friend class TerminatedState;

 private:
  CameraSet cameras_{};
  rx::subscription on_swap_created_{};
  rx::subscription on_pre_frame_{};

  void Register(Camera* rhs);
  void Deregister(Camera* rhs);
  void UpdateAllCameras();
  void FinalizeAllCameras();

 public:
  CameraManager();
  ~CameraManager();

  auto GetNumberOfRegisteredCameras() const -> uint64_t {
    return cameras_.size();
  }

  auto VisitAllCameras(CameraPredicate vis) const -> bool;
  auto VisitAllCameras(CameraVisitor* vis) const -> bool;
};

auto GetCamera() -> Camera*;
auto HasCamera() -> bool;
void InitCameras();
void InitCameraManager();
auto GetCameraManager() -> CameraManager*;
auto IsCameraManagerInitiailized() -> bool;
}  // namespace prt

#endif  // PRT_CAMERA_MANAGER_H
