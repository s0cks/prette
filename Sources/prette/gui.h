#ifndef PRT_GUI_H
#define PRT_GUI_H

#include <imgui_freetype.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <implot.h>

#include <glm/fwd.hpp>

#include "prette/gfx.h"

namespace prt::gui {
struct PushConstBlock {
  glm::vec2 scale{};
  glm::vec2 translate{};
};

void Init();
void Shutdown();
auto Update(const glm::u32vec2& size) -> bool;
}  // namespace prt::gui

#endif  // PRT_GUI_H
