#ifndef PRT_SHAPE_H
#error "Please #include <prette/shape.h> instead."
#endif //PRT_SHAPE_H

#ifndef PRT_RECTANGLE_H
#define PRT_RECTANGLE_H

#include <iostream>
#include "prette/shape.h"
#include "prette/platform.h"

namespace prt {
  class Rectangle : public Shape {
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
    Rectangle(const Rectangle& rhs) = default;
    ~Rectangle() override = default;

    const Point& GetPos() const {
      return pos_;
    }

    void SetPos(const Point& rhs) {
      pos_ = rhs;
    }

    Point GetTopLeft() const {
      return pos_;
    }

    Point GetBottomRight() const {
      return pos_ + glm::vec2(width_, height_);
    }

    Point GetTopRight() const {
      return Point(pos_[0] + width_, pos_[1]);
    }

    Point GetBottomLeft() const {
      return Point(pos_[0], pos_[1] + height_);
    }

    float GetWidth() const {
      return width_;
    }

    float GetHeight() const {
      return height_;
    }

    bool Contains(const Point& p) const override;
    std::string ToString() const override;

    Rectangle& operator=(const Rectangle& rhs) = default;

    friend std::ostream& operator<<(std::ostream& stream, const Rectangle& rhs) {
      return stream << rhs.ToString();
    }
  };
}

#endif //PRT_RECTANGLE_H