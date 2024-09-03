#ifndef PRT_SHAPE_H
#define PRT_SHAPE_H

#include "prette/glm.h"

namespace prt {
  typedef glm::vec2 Point;

  static inline std::ostream&
  operator<<(std::ostream& stream, const Point& rhs) {
    return stream << glm::to_string(rhs);
  }

  class Shape {
  protected:
    Shape() = default;
  public:
    virtual ~Shape() = default;
    virtual bool Contains(const Point& p) const = 0;
    virtual std::string ToString() const = 0;
  };
}

#include "prette/rectangle.h"

#endif //PRT_SHAPE_H