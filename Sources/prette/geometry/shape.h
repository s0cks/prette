#ifndef PRT_SHAPE_H
#define PRT_SHAPE_H

#include "prette/glm.h"
#include "prette/common.h"

namespace prt {
  using Point = glm::vec2;

  static inline auto
  operator<<(std::ostream& stream, const Point& rhs) -> std::ostream& {
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
}

#endif //PRT_SHAPE_H
