#ifndef PRT_TRANSFORM_H
#define PRT_TRANSFORM_H

#include <utility>

#include "prette/glm.h"
#include "prette/vertex/vertex.h"

namespace prt {
struct Transform {
 public:
  static constexpr const auto kXAxis = glm::vec3(1.0f, 0.0f, 0.0f);
  static constexpr const auto kYAxis = glm::vec3(0.0f, 1.0f, 0.0f);
  static constexpr const auto kZAxis = glm::vec3(0.0f, 0.0f, 1.0f);

 public:
  STD140_VEC3(pos);
  STD140_VEC3(scale);
  STD140_VEC3(rotation);

  auto Apply(glm::mat4 model = glm::mat4(1.0f)) const -> glm::mat4 {
    model = glm::translate(model, pos);
    model = glm::rotate(model, glm::radians(rotation.x), kXAxis);
    model = glm::rotate(model, glm::radians(rotation.x), kYAxis);
    model = glm::rotate(model, glm::radians(rotation.x), kZAxis);
    model = glm::scale(model, scale);
    return std::move(model);
  }

  auto operator()(glm::mat4 model = glm::mat4(1.0f)) const -> glm::mat4 {
    return Apply(std::move(model));
  }

  operator glm::mat4() const {
    return Apply();
  }
};
}  // namespace prt

#endif  // PRT_TRANSFORM_H
