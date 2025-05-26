#ifndef PRT_ENGINE_TPS_CHART_H
#define PRT_ENGINE_TPS_CHART_H

#include <cstdint>
#include <yoga/YGConfig.h>

#include "prette/gui/gui_component.h"
#include "prette/rx.h"
#include "prette/series.h"

namespace prt {
class EngineTicksPerSecondChart : public gui::Component {
 private:
  NumericSeries<uint16_t> tpsseries_{};
  TimeSeries<> avg_{};
  TimeSeries<> min_{};
  TimeSeries<> max_{};
  rx::subscription on_stats_{};

 public:
  EngineTicksPerSecondChart();
  ~EngineTicksPerSecondChart();
  void Render() override;
  auto CreateYogaNode() const -> YGNodeRef;
};
}  // namespace prt

#endif  // PRT_ENGINE_TPS_CHART_H
