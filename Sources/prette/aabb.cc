#include "prette/aabb.h"
#include <glm/gtx/component_wise.hpp>

namespace prt::aabb {
  void AABB::ExpandBy(const float value) {
    if(invalid())
      return;
    min -= glm::vec3(value);
    max += glm::vec3(value);
  }

  void AABB::ExpandToInclude(const glm::vec3& value) {
    if(invalid()) {
      min = value;
      max = value;
      return;
    }

    min = glm::min(value, min);
    max = glm::max(value, max);
  }

  void AABB::ExpandToInclude(const glm::vec3& pos, const float radius) {
    glm::vec3 r(radius);
    if(invalid()) {
      min = pos - r;
      max = pos + r;
      return;
    }

    min = glm::min(pos - r, min);
    max = glm::max(pos + r, max);
  }

  void AABB::ExpandToInclude(const AABB& rhs) {
    if(rhs.invalid())
      return;
    ExpandToInclude(rhs.min);
    ExpandToInclude(rhs.max);
  }

  glm::vec3 AABB::GetDiagonal() const {
    if(invalid())
      return glm::vec3(0.0f);
    return max - min;
  }

  float AABB::GetLongestEdge() const {
    return glm::compMax(GetDiagonal());
  }

  float AABB::GetShortestEdge() const {
    return glm::compMin(GetDiagonal());
  }

  glm::vec3 AABB::GetCenter() const {
    if(invalid())
      return glm::vec3(0.0f);
    const auto diagonal = GetDiagonal();
    return min + (diagonal * 0.5f);
  }

  void AABB::Translate(const glm::vec3& value) {
    if(invalid())
      return;
    min += value;
    max += value;
  }

  void AABB::Scale(const glm::vec3& scale, const glm::vec3& origin) {
    if(invalid())
      return;
    min -= origin;
    max -= origin;
    min *= scale;
    max *= scale;
    min += origin;
    max += origin;
  }

  bool AABB::Overlaps(const AABB& rhs) const {
    if(invalid() || rhs.invalid())
      return false;
    if(rhs.min.x > max.x || rhs.max.x < min.x)
      return false;
    else if(rhs.min.y > max.y || rhs.max.y < min.y)
      return false;
    else if(rhs.min.z > max.z || rhs.max.z < min.z)
      return false;
    return true;
  }

  Intersection AABB::Intersect(const AABB& rhs) const {
    if(invalid() || rhs.invalid())
      return kOutside;
    if((max.x < rhs.min.x) || (min.x > rhs.max.x) ||
        (max.y < rhs.min.y) || (min.y > rhs.max.y) ||
        (max.z < rhs.min.z) || (min.z > rhs.max.z))
      return kOutside;
    if((min.x <= rhs.min.x) && (max.x >= rhs.max.x) &&
        (min.y <= rhs.min.y) && (max.y >= rhs.max.y) &&
        (min.z <= rhs.min.z) && (max.z >= rhs.max.z))
      return kInside;
    return kIntersect;
  }

  bool AABB::IsSimilarTo(const AABB& rhs, const float diff) const {
    if(invalid() || rhs.invalid())
      return false;
    
    const auto acceptable_diff = ((GetDiagonal() + rhs.GetDiagonal()) / 2.0f) * diff;
    auto min_diff = min - rhs.min;
    min_diff = glm::vec3(fabs(min_diff.x), fabs(min_diff.y), fabs(min_diff.z));
    if(min_diff.x > acceptable_diff.x)
      return false;
    else if(min_diff.y > acceptable_diff.y)
      return false;
    else if(min_diff.z > acceptable_diff.z)
      return false;
    auto max_diff = max - rhs.max;
    max_diff = glm::vec3(fabs(max_diff.x), fabs(max_diff.y), fabs(max_diff.z));
    if(max_diff.x > acceptable_diff.x)
      return false;
    else if(max_diff.y > acceptable_diff.y)
      return false;
    else if(max_diff.z > acceptable_diff.z)
      return false;
    return true;
  }
}