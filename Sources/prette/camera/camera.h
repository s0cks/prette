#ifndef PRT_CAMERA_H
#define PRT_CAMERA_H

#include "prette/glm.h"
#include "prette/uuid.h"
#include "prette/object.h"

namespace gpu {
  struct CameraData {
    glm::mat4 projection;
    glm::mat4 view;
  };
}

namespace prt {
  namespace camera {
    class Camera : public Object {
      DEFINE_NON_COPYABLE_TYPE(Camera);
    private:
      UUID id_;
    protected:
      explicit Camera(const Metadata& meta = {}):
        Object(meta),
        id_() {
      }
    public:
      ~Camera() override = default;

      auto GetId() const -> const UUID& {
        return id_;
      }

      virtual auto GetProjection() const -> const glm::mat4& = 0;
      virtual auto GetView() const -> const glm::mat4& = 0;
    };

    static inline auto
    IsCameraUri(const std::string& value) -> bool {
      return StartsWith(value, "camera://")
          || StartsWith(value, "file://");
    }
  }
  using camera::Camera;
}

#endif //PRT_CAMERA_H