#ifndef PRT_ENGINE_STATS_H
#define PRT_ENGINE_STATS_H

#include "prette/series.h"

namespace prt::engine {
  struct Sample {
    uint64_t duration;

    friend std::ostream& operator<<(std::ostream& stream, const Sample& rhs) {
      stream << "engine::Sample(";
      stream << "duration=" << rhs.duration;
      stream << ")";
      return stream;
    }
  public:
    struct DurationComparator {
    public:
      bool operator()(const Sample& lhs, const Sample& rhs) {
        return lhs.duration < rhs.duration;
      }
    };
  };

  static constexpr const uint64_t kDefaultNumberOfEngineSamples = 10;
  class SampleSeries : public Series<Sample, kDefaultNumberOfEngineSamples> {
  public:
    SampleSeries() = default;
    ~SampleSeries() override = default;

    uint64_t GetDurationTotal() const {
      uint64_t total = 0;
      for(auto& sample : data_)
        total += sample.duration;
      return total;
    }

    uint64_t GetDurationAvg() const {
      return GetDurationTotal() / data_.size();
    }

    uint64_t GetDurationMax() const {
      return std::max_element(data_.begin(), data_.end(), Sample::DurationComparator{})->duration;
    }

    uint64_t GetDurationMin() const {
      return std::min_element(data_.begin(), data_.end(), Sample::DurationComparator{})->duration;
    }

    uint64_t GetSize() const {
      return data_.size();
    }

    friend SampleSeries& operator<<(SampleSeries& lhs, const Sample& rhs) {
      lhs.data_.put(rhs);
      return lhs;
    }
  };
}

#endif //PRT_ENGINE_STATS_H