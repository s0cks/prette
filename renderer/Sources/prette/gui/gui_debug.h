#ifndef PRT_GUI_DEBUG_H
#define PRT_GUI_DEBUG_H

#include "prette/gui/charts/engine_tps_chart.h"
#include "prette/gui/gui.h"

namespace prt {
class GuiDebug : public Gui {
 private:
  EngineTicksPerSecondChart engine_tps_{};

 protected:
  void RenderChildren() override;
  void OnBoundsChanged() override;
  auto CreateYogaNode() const -> YGNodeRef override;

 public:
  GuiDebug();
  ~GuiDebug() override;

  auto GetGuiName() const -> const char* override {
    return "Debug";
  }
};
}  // namespace prt

#endif  // PRT_GUI_DEBUG_H
