#ifndef PRT_GUI_COMPONENT_H
#define PRT_GUI_COMPONENT_H

#include <yoga/YGConfig.h>
#include <yoga/YGNodeLayout.h>

#include "prette/glm.h"

namespace prt::gui {
class Component {
 private:
  glm::vec4 bounds_{};

 protected:
  Component() = default;

  virtual void OnBoundsChanged() {
    // do nothing
  }

 public:
  virtual ~Component() = default;

  virtual void Render() {
    // do nothing
  }

  auto GetBounds() const -> const glm::vec4& {
    return bounds_;
  }

  void SetBounds(const glm::vec4& rhs) {
    bounds_ = rhs;
    OnBoundsChanged();
  }

  inline void SetBounds(const YGNodeRef rhs) {
    return SetBounds(glm::vec4{
        YGNodeLayoutGetLeft(rhs),
        YGNodeLayoutGetTop(rhs),
        YGNodeLayoutGetWidth(rhs),
        YGNodeLayoutGetHeight(rhs),
    });
  }
};
}  // namespace prt::gui

#endif  // PRT_GUI_COMPONENT_H
