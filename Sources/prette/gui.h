#ifndef PRT_GUI_H
#define PRT_GUI_H

#include <vulkan/vulkan.h>

#include "prette/dimension.h"

namespace prt::gui {
void Init(const Dimension& size);
void Shutdown();
void Render(VkCommandBuffer cbuff);
}  // namespace prt::gui

#endif  // PRT_GUI_H
