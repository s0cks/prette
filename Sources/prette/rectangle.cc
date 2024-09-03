#include "prette/shape.h"
#include <sstream>

namespace prt {
  bool Rectangle::Contains(const Point& p) const {
    const auto bottom_right = GetBottomRight();
    return pos_.x <= p.x
        && pos_.y <= p.y
        && bottom_right.x >= p.x
        && bottom_right.y >= p.y;
  }

  std::string Rectangle::ToString() const {
    std::stringstream ss;
    ss << "Rectangle(";
    ss << "pos=" << GetPos() << ", ";
    ss << "width=" << GetWidth() << ", ";
    ss << "height=" << GetHeight();
    ss << ")";
    return ss.str();
  }
}