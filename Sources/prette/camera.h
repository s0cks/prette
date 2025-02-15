#ifndef PRT_CAMERA_H
#define PRT_CAMERA_H

#include "prette/glm.h"

namespace prt {
struct CameraData {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;
};
}  // namespace prt

#endif  // PRT_CAMERA_H
