#ifndef PRT_GUI_VIEWPORT_H
#define PRT_GUI_VIEWPORT_H

#include "prette/gui.h"

namespace prt {
class GuiViewport : public Gui {
 protected:
  void Update() override;
  void Render() override;

 public:
  GuiViewport();
  ~GuiViewport() override = default;

  auto GetGuiName() const -> const char* override {
    return "Viewport";
  }

 public:
  static void Init();
};
}  // namespace prt

#endif  // PRT_GUI_VIEWPORT_H
