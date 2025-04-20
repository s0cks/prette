#ifndef PRT_GUI_DEBUG_H
#define PRT_GUI_DEBUG_H

#include "prette/gui.h"

namespace prt {
class GuiDebug : public Gui {
 protected:
  void Update() override;
  void Render() override;

 public:
  GuiDebug();
  ~GuiDebug() override;

  auto GetGuiName() const -> const char* override {
    return "Debug";
  }

 public:
  static void Init();
};
}  // namespace prt

#endif  // PRT_GUI_DEBUG_H
