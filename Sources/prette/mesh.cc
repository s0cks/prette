#include "prette/mesh.h"

namespace prt {
void Mesh::Scale(const glm::vec3& scale) {
  model_ = glm::scale(model_, scale);
}

void Mesh::Rotate(const float angle, const glm::vec3& axis) {
  model_ = glm::rotate(model_, glm::radians(angle), axis);
}

void Mesh::Translate(const glm::vec3& translation) {
  model_ = glm::translate(model_, translation);
}
}  // namespace prt