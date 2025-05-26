#ifndef PRT_RECT_H
#define PRT_RECT_H

#include <utility>
#include <vector>

#include "prette/common.h"
#include "prette/vk.h"

namespace prt {
class Rect {
  DEFINE_DEFAULT_COPYABLE_TYPE(Rect);

 public:
  static inline auto IsBelow(const VkRect2D& lhs, const VkRect2D& rhs) -> bool {
    return lhs.offset.x == rhs.offset.x && lhs.extent.width == rhs.extent.width &&
           lhs.offset.x + lhs.extent.height == rhs.offset.y;
  }

  static inline auto IsAbove(const VkRect2D& lhs, const VkRect2D& rhs) -> bool {
    return lhs.offset.x == rhs.offset.x && lhs.extent.width == rhs.extent.width &&
           rhs.offset.y + rhs.extent.height == lhs.offset.y;
  }

  static inline auto IsRight(const VkRect2D& lhs, const VkRect2D& rhs) -> bool {
    return lhs.offset.y == rhs.offset.y && lhs.extent.height == rhs.extent.height &&
           lhs.offset.x + lhs.extent.width == rhs.offset.x;
  }

  static inline auto IsLeft(const VkRect2D& lhs, const VkRect2D& rhs) -> bool {
    return lhs.offset.y == rhs.offset.y && lhs.extent.height == rhs.extent.height &&
           rhs.offset.x + rhs.extent.width == lhs.offset.x;
  }

  static inline auto IsAdjacent(const VkRect2D& lhs, const VkRect2D& rhs) -> bool {
    return IsBelow(lhs, rhs) || IsAbove(lhs, rhs) || IsRight(lhs, rhs) || IsLeft(lhs, rhs);
  }

  static inline auto Combine(VkRect2D& lhs, const VkRect2D& rhs) -> bool {
    bool changed = false;
    if (IsBelow(lhs, rhs)) {
      lhs.extent.height += rhs.extent.height;
      changed = true;
    } else if (IsAbove(lhs, rhs)) {
      lhs.offset.y -= static_cast<int>(rhs.extent.height);
      lhs.extent.height += rhs.extent.height;
      changed = true;
    } else if (IsRight(lhs, rhs)) {
      lhs.extent.width += rhs.extent.width;
      changed = true;
    } else if (IsLeft(lhs, rhs)) {
      lhs.offset.x -= static_cast<int>(rhs.extent.width);
      lhs.extent.width += rhs.extent.width;
      changed = true;
    }
    return changed;
  }

 private:
  VkRect2D bounds_;

 public:
  Rect(const VkRect2D bounds = {}) :
    bounds_(std::move(bounds)) {}
  ~Rect() = default;

  auto GetBounds() const -> const VkRect2D& {
    return bounds_;
  }

  auto GetExtent() const -> const VkExtent2D& {
    return bounds_.extent;
  }

  auto GetWidth() const -> uint32_t {
    return bounds_.extent.width;
  }

  auto GetHeight() const -> uint32_t {
    return bounds_.extent.height;
  }

  auto GetOffset() const -> const VkOffset2D& {
    return bounds_.offset;
  }

  auto GetOffsetX() const -> int32_t {
    return bounds_.offset.x;
  }

  auto GetOffsetY() const -> int32_t {
    return bounds_.offset.y;
  }

  operator VkRect2D() const {
    return bounds_;
  }
};

using RectList = std::vector<Rect>;

static inline auto FindAdjacent(const RectList& rects, const Rect& rhs) -> RectList::const_iterator {
  for (auto it = std::begin(rects); it != std::end(rects); it++) {
    if (Rect::IsAdjacent(rhs, (*it)))
      return it;
  }
  return std::end(rects);
}

static inline auto FindBestFit(const RectList& rects, const VkExtent2D& rhs) -> RectList::const_iterator {
  for (auto it = std::begin(rects); it != std::end(rects); it++) {
    if ((*it).GetWidth() >= rhs.width && (*it).GetHeight() >= rhs.height)
      return it;
  }
  return std::end(rects);
}
}  // namespace prt

#endif  // PRT_RECT_H
