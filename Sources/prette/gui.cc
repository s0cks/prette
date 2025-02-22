#include "prette/gui.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <implot.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <rx-coordination.hpp>
#include <vector>

#include "prette/engine.h"
#include "prette/flags.h"
#include "prette/gfx.h"
#include "prette/gfx_driver.h"
#include "prette/gui_camera.h"
#include "prette/gui_debug.h"
#include "prette/gui_renderer.h"
#include "prette/gui_viewport.h"
#include "prette/lua.h"
#include "prette/mouse.h"
#include "prette/renderer.h"
#include "prette/series.h"
#include "prette/shader.h"
#include "prette/window.h"

namespace prt {
Gui::Gui(std::string name) :
  name_(std::move(name)) {}

Gui::~Gui() {}

namespace gui {
struct VkFont {
  VkImage image{};
  VkDeviceMemory memory{};
  VkImageView view{};
};

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
static inline void InitStyle() {
  ImGui::StyleColorsDark();
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

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

static void Destroy(const Driver* driver) {
  ASSERT(driver);
}

void Init() {
  OnDriverInitEvent().subscribe([](DriverInitEvent* event) {
    ASSERT(event);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    const auto window = GetAppWindow();
    ASSERT(window);
    ImGui_ImplGlfw_InitForVulkan(window->GetHandle(), true);
  });
  engine::OnPostInitEvent().subscribe([](engine::PostInitEvent* event) {
    ASSERT(event);
    guis_.push_back(new GuiViewport());
    guis_.push_back(new GuiDebug());
    guis_.push_back(new GuiCamera());
  });
  engine::OnTickEvent().subscribe([](engine::TickEvent* event) {
    ASSERT(event);
    const auto window = GetAppWindow();
    ASSERT(window);
    const auto size = window->GetSize();
    if (!Update(size.data())) {
      // update more?
    }
  });
  OnDestroyingDriverEvent().subscribe([](DestroyingDriverEvent* event) {
    ASSERT(event);
    const auto driver = Driver::Get();
    ASSERT(driver);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
  });
  GuiViewport::Init();
  GuiDebug::Init();
}
}  // namespace gui
}  // namespace prt