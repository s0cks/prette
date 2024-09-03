#ifndef PRT_AABB_H
#define PRT_AABB_H

#include "prette/gfx.h"

namespace prt::aabb {
  enum Intersection {
    kInside,
    kOutside,
    kIntersect,
  };

  struct AABB {
    glm::vec3 min;
    glm::vec3 max;
  public:
    void reset() {
      min = glm::vec3(1.0f);
      max = glm::vec3(-1.0f);
    }

    bool invalid() const {
      return min.x > max.x || min.y > max.y || min.z > max.z;
    }

    void ExpandBy(const float value);
    void ExpandToInclude(const glm::vec3& value);
    void ExpandToInclude(const glm::vec3& pos, const float radius);
    void ExpandToInclude(const AABB& value);
    void Translate(const glm::vec3& v);
    void Scale(const glm::vec3& scale, const glm::vec3& origin);
    bool Overlaps(const AABB& rhs) const;
    bool IsSimilarTo(const AABB& rhs, const float diff = 0.5f) const;
    Intersection Intersect(const AABB& rhs) const; 
    glm::vec3 GetCenter() const;
    glm::vec3 GetDiagonal() const;
    float GetLongestEdge() const;
    float GetShortestEdge() const;
  };

  static void Translate(const glm::vec3 rhs);
  static void Scale(const glm::vec3& scale, const glm::vec3& origin);
}

#endif //PRT_AABB_H