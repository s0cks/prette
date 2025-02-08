#ifndef PRT_TICK_PROFILER_H
#define PRT_TICK_PROFILER_H

#include "prette/common.h"
#include "prette/tick.h"

namespace prt {
struct TickStats {
  double avg;
  double min;
  double max;

  friend auto operator<<(std::ostream& stream, const TickStats& rhs) -> std::ostream& {
    using ns = units::time::nanosecond_t;
    return stream << "avg=" << ns(rhs.avg) << ", min=" << ns(rhs.min) << ", max=" << ns(rhs.max);
  }
};

class TickProfiler {
 public:
  static constexpr const auto kTickProfilerMaxCount = 1000;
  static constexpr const auto kTickProfilerRate = std::chrono::seconds(1);
  DEFINE_NON_COPYABLE_TYPE(TickProfiler);

 private:
  rx::subject<TickStats> stats_{};
  rx::subscription on_delta_{};

  template <typename N>
  static inline auto Sum(const std::vector<N>& values) -> N {
    if (values.empty())
      return 0;
    return std::reduce(std::begin(values), std::end(values), 0);
  }

  template <typename N>
  static inline auto Average(const std::vector<N>& values) -> double {
    const auto total = Sum(values);
    return total / values.size();
  }

  template <typename T, typename Container>
  static inline auto max_or(const Container& container, const T default_value) -> T {
    if (container.empty())
      return default_value;
    const auto max_pos = std::ranges::max_element(container);
    return max_pos != std::end(container) ? (*max_pos) : default_value;
  }

  template <typename T, typename Container>
  static inline auto min_or(const Container& container, const T default_value) -> T {
    if (container.empty())
      return default_value;
    const auto max_pos = std::ranges::max_element(container);
    return max_pos != std::end(container) ? (*max_pos) : default_value;
  }

 public:
  explicit TickProfiler(rx::observable<uint64_t> deltas) {
    on_delta_ = deltas.buffer_with_time_or_count(kTickProfilerRate, kTickProfilerMaxCount, rx::observe_on_new_thread())
                    .skip(3)
                    .map([this](std::vector<uint64_t> deltas) {
                      const auto avg = Average(deltas);
                      const auto min = static_cast<double>(min_or(deltas, 0));
                      const auto max = static_cast<double>(max_or(deltas, 0));
                      return TickStats{.avg = avg, .min = min, .max = max};
                    })
                    .subscribe(stats_.get_subscriber());
  }
  ~TickProfiler() {
    on_delta_.unsubscribe();
  }

  auto OnStats() const -> rx::observable<TickStats> {
    return stats_.get_observable();
  }
};
}  // namespace prt

#endif  // PRT_TICK_PROFILER_H
