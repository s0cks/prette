#include "prette/gui/gui.h"

#include <imgui.h>
#include <string>
#include <utility>
#include <yoga/YGConfig.h>
#include <yoga/YGEnums.h>
#include <yoga/YGNode.h>
#include <yoga/YGNodeLayout.h>
#include <yoga/YGNodeStyle.h>
#include <yoga/YGValue.h>
#include <yoga/Yoga.h>

namespace prt {
Gui::Gui(std::string name) :
  name_(std::move(name)) {}

auto Gui::CreateYogaNode() const -> YGNodeRef {
  YGNodeRef node = YGNodeNew();
  YGNodeStyleSetFlexGrow(node, 1.0f);
  YGNodeStyleSetMargin(node, YGEdgeRight, 10.0f);
  return node;
}

void Gui::Render() {
  ImGui::SetNextWindowPos(
      ImVec2{
          bounds_.x,
          bounds_.y,
      },
      ImGuiCond_Once);
  ImGui::SetNextWindowSize(
      ImVec2{
          bounds_.z,
          bounds_.w,
      },
      ImGuiCond_Once);
  ImGui::Begin(GetGuiName(), nullptr, GetWindowFlags());
  RenderChildren();
  ImGui::End();
}
}  // namespace prt