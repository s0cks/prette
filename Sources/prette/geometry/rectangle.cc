#include "prette/geometry/rectangle.h"
#include <sstream>

namespace prt {
  auto Rectangle::Contains(const Point& p) const -> bool {
    const auto bottom_right = GetBottomRight();
    return pos_.x <= p.x
        && pos_.y <= p.y
        && bottom_right.x >= p.x
        && bottom_right.y >= p.y;
  }

  auto Rectangle::ToString() const -> std::string {
    std::stringstream ss;
    ss << "Rectangle(";
    ss << "pos=" << GetPos() << ", ";
    ss << "width=" << GetWidth() << ", ";
    ss << "height=" << GetHeight();
    ss << ")";
    return ss.str();
  }
}