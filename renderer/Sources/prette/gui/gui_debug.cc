#include "prette/gui/gui_debug.h"

#include <imgui.h>
#include <units.h>

#include "prette/assertions.h"
#include "prette/camera.h"
#include "prette/gui/gui.h"

namespace prt {

GuiDebug::GuiDebug() :
  Gui("Debug") {
  const auto camera = GetCamera();
  ASSERT(camera);
}

GuiDebug::~GuiDebug() = default;

void GuiDebug::OnBoundsChanged() {
  const auto root = YGNodeNew();
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionColumn);
  YGNodeStyleSetFlexWrap(root, YGWrapWrap);
  YGNodeStyleSetMarginPercent(root, YGEdgeAll, 10.0f);
  YGNodeStyleSetAlignItems(root, YGAlignCenter);

  const auto engine_tps_node = engine_tps_.CreateYogaNode();
  YGNodeInsertChild(root, engine_tps_node, 0);

  YGNodeCalculateLayout(root, bounds_.z, bounds_.w, YGDirectionLTR);

  engine_tps_.SetBounds(engine_tps_node);
  YGNodeFreeRecursive(root);
}

auto GuiDebug::CreateYogaNode() const -> YGNodeRef {
  const auto node = YGNodeNew();
  YGNodeStyleSetWidthPercent(node, 25.0f);
  return node;
}

void GuiDebug::RenderChildren() {
  engine_tps_.Render();
}

}  // namespace prt