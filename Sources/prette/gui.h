#ifndef PRT_GUI_H
#define PRT_GUI_H

#include <imgui.h>

#include "prette/dimension.h"
#include "prette/gfx.h"

namespace prt::gui {
void Init(const Dimension& size);
void Shutdown();
void Render(VkCommandBuffer cbuff);
}  // namespace prt::gui

#endif  // PRT_GUI_H
