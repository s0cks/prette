#ifndef PRT_GUI_VIEWPORT_H
#define PRT_GUI_VIEWPORT_H

#include "prette/glm.h"
#include "prette/gui.h"

namespace prt {
class GuiViewport : public Gui {
 private:
  glm::vec2 pos_{0.0f};
  glm::vec2 size_{0.0f};
  glm::vec2 viewpos_{0.0f};

 protected:
  void Update() override;
  void Render() override;

 public:
  GuiViewport();
  ~GuiViewport() override = default;

  auto GetGuiName() const -> const char* override {
    return "Viewport";
  }

  auto GetPos() const -> const glm::vec2& {
    return pos_;
  }

  auto GetSize() const -> const glm::vec2& {
    return size_;
  }

  auto GetViewPos() const -> const glm::vec2& {
    return viewpos_;
  }

 public:
  static auto IsInitialized() -> bool;
  static auto Get() -> GuiViewport*;
  static auto Init() -> GuiViewport*;
};
}  // namespace prt

#endif  // PRT_GUI_VIEWPORT_H
