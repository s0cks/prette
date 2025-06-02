#include "prette/tile_mesh.h"

#include <cmath>

#include "prette/mesh/mesh.h"
#include "prette/mesh/mesh_builder.h"
#include "prette/vertex/vertex2d.h"

namespace prt {
static const TileMeshClass::VertexArray kTileVertices = {
    bare2d::Vertex{.pos = {-0.5f, -0.5f}},  // top left
    bare2d::Vertex{.pos = {0.5f, -0.5f}},   // top right
    {.pos = {0.5f, 0.5f}},                  // bottom right
    {.pos = {-0.5f, 0.5f}},                 // bottom left
};
static const TileMeshClass::IndexArray kTileIndices = {
    0, 1, 2, 2, 3, 0,
};

static inline auto cross(const glm::vec2& v1, const glm::vec2& v2) -> float {
  return (v1.x * v2.y) - (v1.y * v2.x);
}

static inline auto normalize(const glm::vec2& v) -> glm::vec2 {
  float magnitude = std::sqrt(v.x * v.x + v.y * v.y);
  if (magnitude > 0) {
    return {v.x / magnitude, v.y / magnitude};
  } else {
    return {0, 0};
  }
}

static inline auto CalculateTileMeshNormal() -> glm::vec2 {
  const auto v1 = kTileVertices[1].pos - kTileVertices[0].pos;
  const auto v2 = kTileVertices[2].pos - kTileVertices[0].pos;
  const auto x = cross(v1, v2);
  const auto normal = glm::vec2(0, x > 0 ? 1.0f : -1.0f);
  return normalize(normal);  // TODO: cache this variable
}

static const glm::vec2 kTileNormal = CalculateTileMeshNormal();

auto NewTileMesh() -> vk::IndexedMesh* {
  vk::IndexedMeshBuilder<TileMeshClass> builder{};
  builder.Append(&kTileVertices[0], kTileVertices.size());
  builder.Append(&kTileIndices[0], kTileIndices.size());
  return builder;
}

auto GetTileNormal() -> glm::vec2 {
  return kTileNormal;
}
}  // namespace prt