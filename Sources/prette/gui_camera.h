#ifndef PRT_GUI_CAMERA_H
#define PRT_GUI_CAMERA_H

#include "prette/gui.h"

namespace prt {
class GuiCamera : public Gui {
 protected:
  void Update() override;
  void Render() override;

 public:
  GuiCamera();
  ~GuiCamera() override = default;

  auto GetGuiName() const -> const char* override {
    return "Camera";
  }

 public:
  static void Init();
};
}  // namespace prt

#endif  // PRT_GUI_CAMERA_H
