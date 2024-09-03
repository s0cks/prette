#ifndef PRT_PROJECTION_MATRIX_H
#define PRT_PROJECTION_MATRIX_H

#include "prette/glm.h"

namespace prt {
  class ProjectionMatrix {
  public:
    static void Init();
    static void Set(const glm::mat4 data);
    static glm::mat4 Get();
  };
}

#endif //PRT_PROJECTION_MATRIX_H