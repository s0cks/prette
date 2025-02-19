#include "prette/model.h"

namespace prt {
void Model::Scale(const glm::vec3& scale) {
  for (auto& mesh : meshes_) {
    mesh.Scale(scale);
  }
}

void Model::Rotate(const float angle, const glm::vec3& axis) {
  for (auto& mesh : meshes_) {
    mesh.Rotate(angle, axis);
  }
}

void Model::Translate(const glm::vec3& translation) {
  for (auto& mesh : meshes_) {
    mesh.Translate(translation);
  }
}
}  // namespace prt