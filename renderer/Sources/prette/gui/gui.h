#ifndef PRT_GUI_H
#define PRT_GUI_H

#include <fmt/format.h>
#include <string>

// IWYU pragma: begin_exports
#include <imgui.h>
#include <imgui_freetype.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <implot.h>
#include <yoga/YGConfig.h>
#include <yoga/YGEnums.h>
#include <yoga/YGNode.h>
#include <yoga/YGNodeLayout.h>
#include <yoga/YGNodeStyle.h>
// IWYU pragma: end_exports

#include "prette/assertions.h"
#include "prette/glm.h"
#include "prette/gui/gui_event.h"
#include "prette/lua.h"
#include "prette/rx.h"

namespace prt {
class Gui {
  friend class GuiSystem;

 protected:
  glm::vec4 bounds_{};

 private:
  std::string name_;
  rx::subscription on_tick_{};

 protected:
  explicit Gui(std::string name);

  void LoadLuaScript(const std::string& name) {
    const auto lua = GetLua();
    ASSERT(lua);
    lua->ExecuteScript(name);
  }

  template <typename... Args>
  void InvokeLuaCallback(const std::string& name, Args... args) {
    const auto lua = GetLua();
    ASSERT(lua);
    lua->ExecuteGlobalFunc(name_);
  }

  void SetBounds(const glm::vec4 rhs) {
    bounds_ = rhs;
    OnBoundsChanged();
  }

  virtual void OnBoundsChanged() {
    // do nothing?
  }

  virtual auto GetWindowFlags() const -> ImGuiWindowFlags {
    return ImGuiWindowFlags_NoCollapse;
  }

  virtual void RenderChildren() = 0;

 public:
  virtual ~Gui() = default;
  virtual auto GetGuiName() const -> const char* = 0;

 public:  // TODO: reduce visibility
  virtual void Update() {}
  virtual void Render();
  virtual auto CreateYogaNode() const -> YGNodeRef;  // TODO: reduce visibility
};
}  // namespace prt

#endif  // PRT_GUI_H
