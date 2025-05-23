#include "prette/vertex2d.h"

#include "prette/glm.h"  // IWYU pragma: keep

namespace prt {
namespace tex2d {
auto Vertex::operator==(const Vertex& rhs) const -> bool {
  return pos == rhs.pos && uv == rhs.uv;
}
}  // namespace tex2d

namespace mat2d {
auto Vertex::operator==(const Vertex& rhs) const -> bool {
  return pos == rhs.pos && material == rhs.material;
}
}  // namespace mat2d

namespace color2d {
auto Vertex::operator==(const Vertex& rhs) const -> bool {
  return pos == rhs.pos && color == rhs.color;
}
}  // namespace color2d
}  // namespace prt