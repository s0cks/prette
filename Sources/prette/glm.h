#ifndef PRT_GLM_H
#define PRT_GLM_H

#include <cmath>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace prt {
  static inline auto
  Clamp(glm::vec2& pos, const glm::vec4& bounds) -> bool {
    bool changed = false;
    if(pos.x < bounds[0]) {
      pos.x = bounds[0];
      changed = true;
    } else if(pos.x > bounds[2]) {
      pos.x = bounds[2];
      changed = true;
    } else if(pos.y < bounds[1]) {
      pos.y = bounds[1];
      changed = true;
    } else if(pos.y > bounds[3]) {
      pos.y = bounds[3];
      changed = true;
    }
    return changed;
  }

  static inline auto
  Clamp(glm::vec2& pos, const glm::vec2& size) -> bool {
    return Clamp(pos, glm::vec4(0.0f, 0.0f, size));
  }
}

#endif //PRT_GLM_H