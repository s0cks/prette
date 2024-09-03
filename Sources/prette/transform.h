#ifndef PRT_TRANSFORM_H
#define PRT_TRANSFORM_H

#include "prette/gfx.h"
#include "prette/component/component.h"

namespace prt {
  struct Transform {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    friend std::ostream& operator<<(std::ostream& stream, const Transform& rhs) {
      stream << "physics::Transform(";
      stream << "position=" << glm::to_string(rhs.position) << ", ";
      stream << "rotation=" << glm::to_string(rhs.rotation) << ", ";
      stream << "scale=" << glm::to_string(rhs.scale);
      stream << ")";
      return stream;
    }
  };

  class TransformComponent : public StatefulComponent<Transform> {
  private:
    TransformComponent() = default;
  public:
    ~TransformComponent() override = default;
    DECLARE_COMPONENT(Transform);
  public:
    static void Init();
    static TransformComponent* Get();
  };
}

#endif //PRT_TRANSFORM_H