#ifndef PRT_MESH_H
#define PRT_MESH_H

#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/vk.h"
#include "prette/vk_buffer.h"

namespace prt::vk {
class Mesh {
 private:
  vk::Buffer* vertices_ = nullptr;

 public:
  explicit Mesh(vk::Buffer* vertices) :
    vertices_(vertices) {
    ASSERT_INITIALIZED(vertices_);
  }
  virtual ~Mesh() {
    delete vertices_;
  }

  auto GetVertexBuffer() const -> vk::Buffer* {
    return vertices_;
  }

  virtual auto IsInitialized() const -> bool {
    return vk::IsInitialized(vertices_);
  }
};

class IndexedMesh : public Mesh {
 private:
  vk::Buffer* indices_ = nullptr;

 public:
  explicit IndexedMesh(vk::Buffer* vertices, vk::Buffer* indices) :
    Mesh(vertices),
    indices_(indices) {}
  ~IndexedMesh() override {
    delete indices_;
  }

  auto GetIndexBuffer() const -> vk::Buffer* {
    return indices_;
  }

  auto IsInitialized() const -> bool override {
    return Mesh::IsInitialized() && vk::IsInitialized(indices_);
  }
};
}  // namespace prt::vk

#endif  // PRT_MESH_H
