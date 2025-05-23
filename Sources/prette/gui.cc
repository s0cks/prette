#include "prette/gui.h"

#include <string>
#include <utility>
#include <vector>

#include "prette/common.h"
#include "prette/engine.h"
#include "prette/engine_event.h"
#include "prette/glm.h"
#include "prette/gui_camera.h"
#include "prette/gui_debug.h"
#include "prette/gui_viewport.h"
#include "prette/vk.h"

namespace prt {
Gui::Gui(std::string name) :
  name_(std::move(name)) {}

namespace gui {
struct VkFont {
  VkImage image{};
  VkDeviceMemory memory{};
  VkImageView view{};
};

static inline void InitStyle() {
  ImGui::StyleColorsDark();
}

static std::vector<Gui*> guis_{};

auto Update(const glm::u32vec2& size) -> bool {
  auto& io = ImGui::GetIO();
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  for (const auto& gui : guis_) {
    gui->Update();
    gui->Render();
  }

  ImGui::Render();
  return true;
}

void Init() {
  OnPostInitEvent().subscribe([](PostInitEvent* event) {
    ASSERT(event);
    guis_.push_back(GuiViewport::Init());
    guis_.push_back(new GuiDebug());
    guis_.push_back(new GuiCamera());
  });
  GuiDebug::Init();
}
}  // namespace gui
}  // namespace prt