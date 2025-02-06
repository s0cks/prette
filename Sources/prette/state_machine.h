#ifndef PRT_STATE_MACHINE_H
#define PRT_STATE_MACHINE_H

#include "prette/counter.h"
#include "prette/series.h"

namespace prt {
static constexpr const auto kDefaultStateSeriesCapacity = 10;
class State {
  using DurationSeries = TimeSeries<kDefaultStateSeriesCapacity>;

 private:
  DurationSeries duration_;

 protected:
  State() = default;

  virtual void Run() = 0;
  virtual void Stop() = 0;

  void AppendDuration(const uint64_t value) {
    duration_.Append(value);
  }

 public:
  virtual ~State() = default;
  virtual auto GetName() const -> const char* = 0;

  auto GetDurationSeries() const -> const DurationSeries& {
    return duration_;
  }
};
}  // namespace prt

#endif  // PRT_STATE_MACHINE_H