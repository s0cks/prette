#ifndef PRT_STATE_MACHINE_H
#define PRT_STATE_MACHINE_H

#include "prette/series.h"
#include "prette/counter.h"

namespace prt {
  static constexpr const auto kDefaultStateSeriesCapacity = 10;
  class State {
    typedef TimeSeries<kDefaultStateSeriesCapacity> DurationSeries;
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
    virtual const char* GetName() const = 0;

    const DurationSeries& GetDurationSeries() const {
      return duration_;
    }
  };
}

#endif //PRT_STATE_MACHINE_H