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
    protected:
      UUID id_;

      explicit Camera(const Metadata& meta = {}):
        Object(meta),
        id_() {
      }
    public:
      virtual ~Camera() = default;

      const UUID& GetId() const {
        return id_;
      }

      virtual const glm::mat4& GetProjection() const = 0;
      virtual const glm::mat4& GetView() const = 0;
    };

    static inline bool
    IsCameraUri(const std::string& value) {
      return StartsWith(value, "camera://")
          || StartsWith(value, "file://");
    }
  }
  using camera::Camera;
}

#endif //PRT_CAMERA_H