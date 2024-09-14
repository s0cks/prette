#ifndef PRT_RECTANGLE_H
#define PRT_RECTANGLE_H

#include <iostream>
#include "prette/geometry/shape.h"

namespace prt {
  class Rectangle : public Shape { // NOLINT(cppcoreguidelines-pro-type-member-init)
    DEFINE_DEFAULT_COPYABLE_TYPE(Rectangle);
  private:
    Point pos_;
    uword width_;
    uword height_;
  public:
    Rectangle() = default;
    Rectangle(const Point& pos, const uword width, const uword height):
      Shape(),
      pos_(pos),
      width_(width),
      height_(height) {
    }
    Rectangle(const uword xPos, const uword yPos, const uword width, const uword height):
      Rectangle(Point(xPos, yPos), width, height) {
    }
    ~Rectangle() override = default;

    auto GetPos() const -> const Point& {
      return pos_;
    }

    void SetPos(const Point& rhs) {
      pos_ = rhs;
    }

    auto GetTopLeft() const -> Point {
      return pos_;
    }

    auto GetBottomRight() const -> Point {
      return pos_ + glm::vec2(width_, height_);
    }

    auto GetTopRight() const -> Point {
      return { pos_[0] + width_, pos_[1] };
    }

    auto GetBottomLeft() const -> Point {
      return { pos_[0], pos_[1] + height_ };
    }

    auto GetWidth() const -> uword {
      return width_;
    }

    auto GetHeight() const -> uword {
      return height_;
    }

    auto Contains(const Point& p) const -> bool override;
    auto ToString() const -> std::string override;

    friend auto operator<<(std::ostream& stream, const Rectangle& rhs) -> std::ostream& {
      return stream << rhs.ToString();
    }
  };
}

#endif //PRT_RECTANGLE_H