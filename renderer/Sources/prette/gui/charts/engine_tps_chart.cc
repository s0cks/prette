#include "prette/gui/charts/engine_tps_chart.h"

#include <array>
#include <cstdint>
#include <cstdio>
#include <imgui.h>
#include <implot.h>
#include <units.h>
#include <yoga/YGConfig.h>
#include <yoga/YGEnums.h>
#include <yoga/YGNode.h>
#include <yoga/YGNodeLayout.h>
#include <yoga/YGNodeStyle.h>

#include "prette/assertions.h"
#include "prette/engine/engine.h"
#include "prette/tick_profiler.h"
#include "prette/uv/utils.h"

namespace prt {
static const std::array<uint64_t, 10> kTicksSeries = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
};

static inline auto TimeFormatter(double value, char* buff, int size, void*) -> int {
  const auto str = units::time::to_string(units::time::millisecond_t(value));
  return snprintf(buff, size, "%s", str.c_str());  // NOLINT(cppcoreguidelines-pro-type-vararg)
}

EngineTicksPerSecondChart::EngineTicksPerSecondChart() {
  on_stats_ = GetEngine()->OnTickProfilerStats().subscribe(([this](const TickStats stats) {
    tpsseries_.Append(GetEngine()->GetTicksPerSecond().per_sec());
    avg_.Append(static_cast<uint64_t>(stats.avg) / NSEC_PER_MSEC);
    min_.Append(static_cast<uint64_t>(stats.min) / NSEC_PER_MSEC);
    max_.Append(static_cast<uint64_t>(stats.max) / NSEC_PER_MSEC);
  }));
}

EngineTicksPerSecondChart::~EngineTicksPerSecondChart() {
  on_stats_.unsubscribe();
}

void EngineTicksPerSecondChart::Render() {
  const auto& bounds = GetBounds();
  ImGui::Indent(bounds.x);
  const auto engine = GetEngine();
  ASSERT(engine);
  ImGui::Text("current tps:");  // NOLINT(cppcoreguidelines-pro-type-vararg)
  ImGui::SameLine();
  ImGui::Text("%llu", engine->GetTicksPerSecond().per_sec());  // NOLINT(cppcoreguidelines-pro-type-vararg)
  if (ImPlot::BeginPlot("Engine Ticks per Second", ImVec2{bounds.z, bounds.w})) {
    ImPlot::SetupAxis(ImAxis_Y1, "Tick(s) per Second", ImPlotAxisFlags_Opposite);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 150);

    ImPlot::SetupAxis(ImAxis_Y2, "Duration (ms)");
    ImPlot::SetupAxisFormat(ImAxis_Y2, TimeFormatter);
    ImPlot::SetupAxisLimits(ImAxis_Y2, 0, 50);

    ImPlot::PlotBars("Ticks", tpsseries_.begin(), 10, 0.4);
    ImPlot::PlotLine("Avg", kTicksSeries.begin(), avg_.begin(), 10);
    ImPlot::PlotLine("Min", kTicksSeries.begin(), min_.begin(), 10);
    ImPlot::PlotLine("Max", kTicksSeries.begin(), max_.begin(), 10);
    ImPlot::EndPlot();
  }
}

auto EngineTicksPerSecondChart::CreateYogaNode() const -> YGNodeRef {
  YGNodeRef node = YGNodeNew();
  YGNodeStyleSetWidthPercent(node, 100.0f);
  return node;
}
}  // namespace prt