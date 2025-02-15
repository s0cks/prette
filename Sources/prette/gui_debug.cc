#include "prette/gui_debug.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <implot.h>

#include "prette/engine.h"
#include "prette/gui_renderer.h"
#include "prette/series.h"
#include "prette/swapchain.h"
#include "prette/window.h"

namespace prt {
static size_t kRenderTarget = 0;
static std::array<uint64_t, 10> tickseries_ = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
static NumericSeries<uint16_t> tpsseries_{};
static TimeSeries<> avg_{};
static TimeSeries<> min_{};
static TimeSeries<> max_{};

static inline auto TimeFormatter(double value, char* buff, int size, void*) -> int {
  const auto str = units::time::to_string(units::time::millisecond_t(value));
  return snprintf(buff, size, "%s", str.c_str());
}

GuiDebug::GuiDebug() :
  Gui("Debug") {
  Engine::Get()->OnTickProfilerStats().subscribe(([](const TickStats stats) {
    tpsseries_.Append(Engine::Get()->GetTicksPerSecond().per_sec());
    avg_.Append(stats.avg / NSEC_PER_MSEC);
    min_.Append(stats.min / NSEC_PER_MSEC);
    max_.Append(stats.max / NSEC_PER_MSEC);
  }));
}

GuiDebug::~GuiDebug() {}

void GuiDebug::Update() {}

void GuiDebug::Render() {
  const auto window = GetAppWindow();
  ASSERT(window);
  const auto size = window->GetSize();
  ImGui::SetNextWindowPos(ImVec2{0, 0}, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2{size.width() / 4, size.height()});
  ImGui::Begin(GetGuiName());
  const auto target_items = std::array<const char*, 2>{
      "Full Scene",
      "Test",
  };
  if (ImGui::BeginCombo("Target", target_items.at(kRenderTarget), ImGuiComboFlags_HeightSmall)) {
    for (auto idx = 0; idx < target_items.size(); idx++) {
      const auto is_selected = (kRenderTarget == idx);
      if (ImGui::Selectable(target_items.at(idx), is_selected)) {
        kRenderTarget = idx;
      }
      if (is_selected) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }

  const auto engine = Engine::Get();
  ASSERT(engine);
  ImGui::Text("tps");
  ImGui::SameLine();
  ImGui::Text("%llu", engine->GetTicksPerSecond().per_sec());

  if (ImPlot::BeginPlot("Profiler")) {
    ImPlot::SetupAxis(ImAxis_Y1, "Tick(s) per Second", ImPlotAxisFlags_Opposite);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 150);

    ImPlot::SetupAxis(ImAxis_Y2, "Duration (ms)");
    ImPlot::SetupAxisFormat(ImAxis_Y2, TimeFormatter);
    ImPlot::SetupAxisLimits(ImAxis_Y2, 0, 50);

    ImPlot::PlotBars("Ticks", tpsseries_.begin(), 10);
    ImPlot::PlotLine("Avg", tickseries_.begin(), avg_.begin(), 10);
    ImPlot::PlotLine("Min", tickseries_.begin(), min_.begin(), 10);
    ImPlot::PlotLine("Max", tickseries_.begin(), max_.begin(), 10);
    ImPlot::EndPlot();
  }
  ImGui::End();
}

void GuiDebug::Init() {}
}  // namespace prt