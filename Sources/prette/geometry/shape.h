#ifndef PRT_SHAPE_H
#define PRT_SHAPE_H

#include <ostream>
#include <string>

#include "prette/glm.h"

namespace prt {
using Point = glm::vec2;

static inline auto operator<<(std::ostream& stream, const Point& rhs) -> std::ostream& {
  return stream << glm::to_string(rhs);
}

class Shape {
 protected:
  Shape() = default;

 public:
  virtual ~Shape() = default;
  virtual auto Contains(const Point& p) const -> bool = 0;
  virtual auto ToString() const -> std::string = 0;
};
}  // namespace prt

#endif  // PRT_SHAPE_H
