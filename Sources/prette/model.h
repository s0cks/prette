#ifndef PRT_MODEL_H
#define PRT_MODEL_H

#include "prette/mesh.h"

namespace prt {
class Model {
 private:
  std::vector<Mesh> meshes_{};
  uint64_t num_vertices_{};
  uint64_t num_indices_{};

 public:
  Model() = default;
  ~Model() = default;

  auto GetMeshes() const -> const std::vector<Mesh>& {
    return meshes_;
  }

  auto GetNumberOfMeshes() const -> uint64_t {
    return meshes_.size();
  }

  auto GetNumberOfVertices() const -> uint64_t {
    return num_vertices_;
  }

  auto GetNumberOfIndices() const -> uint64_t {
    return num_indices_;
  }

  void Scale(const glm::vec3& scale);
  void Rotate(const float angle, const glm::vec3& axis);
  void Translate(const glm::vec3& translation);

  inline void RotateUp(const float angle) {
    return Rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
  }

  inline void RotateDown(const float angle) {
    return Rotate(angle, glm::vec3(0.0f, -1.0f, 0.0f));
  }
};
}  // namespace prt

#endif  // PRT_MODEL_H
