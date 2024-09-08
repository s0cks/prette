#include "prette/camera/camera_ortho.h"

namespace prt::camera {
  static OrthoCameraSet all_; //NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

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

  auto GetAllOrthoCameras() -> const OrthoCameraSet& {
    return all_;
  }

  auto GetTotalNumberOfOrthoCameras() -> uword {
    return all_.size();
  }
}