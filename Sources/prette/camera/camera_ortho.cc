#include "prette/camera/camera_ortho.h"

#include "prette/thread_local.h"
#include "prette/window/window.h"

#include "prette/render/renderer.h"
#include "prette/keyboard/keyboard.h"

namespace prt::camera {
  static OrthoCameraSet all_;

  static inline void
  Register(OrthoCamera* value) {
    PRT_ASSERT(value);
    const auto [iter,success] = all_.insert(value);
    LOG_IF(ERROR, !success) << "failed to register: " << value->ToString();
  }

  static inline void
  Deregister(OrthoCamera* value) {
    PRT_ASSERT(value);
    const auto removed = all_.erase(value);
    LOG_IF(ERROR, removed != 1) << "failed to deregister: " << value->ToString();
  }

  OrthoCamera::OrthoCamera(const OrthoCameraData& data):
    Camera(),
    data_(data) {
    Register(this);
  }

  OrthoCamera::~OrthoCamera() {
    Deregister(this);
  }

  const OrthoCameraSet& GetAllOrthoCameras() {
    return all_;
  }

  uword GetTotalNumberOfOrthoCameras() {
    return all_.size();
  }
}