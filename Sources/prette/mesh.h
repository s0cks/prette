#ifndef PRT_MESH_H
#define PRT_MESH_H

#include "prette/glm.h"
#include "prette/renderer.h"

namespace prt {
class Mesh {
 private:
  glm::mat4 model_ = glm::mat4(1.0f);
  std::vector<Vertex> vertices_;
  std::vector<uint32_t> indices_;

 public:
  Mesh(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices) :
    vertices_(std::move(vertices)),
    indices_(std::move(indices)) {}
  ~Mesh() = default;

  void Scale(const glm::vec3& scale);
  void Rotate(const float angle, const glm::vec3& axis);
  void Translate(const glm::vec3& translate);

  inline void RotateUp(const float angle) {
    return Rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
  }

  inline void RotateDown(const float angle) {
    return Rotate(angle, glm::vec3(0.0f, -1.0f, 0.0f));
  }
};
}  // namespace prt

#endif  // PRT_MESH_H
