#ifndef PRT_SERIES_H
#define PRT_SERIES_H

#include <ostream>
#include <units.h>

#include "prette/circular_buffer.h"
#include "prette/platform.h"
#include "prette/rx.h"  // IWYU pragma: keep

namespace prt {
template <typename T, const uint64_t Capacity>
class Series {
 protected:
  CircularBuffer<T, Capacity> data_;

  Series() :
    data_() {}

 public:
  virtual ~Series() = default;

  void Append(const T value) {
    data_.put(value);
  }

  auto begin() -> T* {
    return data_.begin();
  }

  auto begin() const -> T* {
    return data_.begin();
  }

  auto end() -> T* {
    return data_.end();
  }

  auto end() const -> T* {
    return data_.end();
  }

  auto count() const -> uint64_t {
    return ToObservable().as_blocking().count();
  }

  auto ToObservable() const -> rx::observable<T> {
    return rx::observable<>::create<T>([this](rx::subscriber<T> s) {
      for (const auto& value : data_) {
        s.on_next(value);
      }
      s.on_completed();
    });
  }

  explicit operator rx::observable<T>() const {
    return ToObservable();
  }
};

template <typename T, const uint64_t Capacity = 10>
class NumericSeries : public Series<uint64_t, Capacity> {
 public:
  NumericSeries() = default;
  ~NumericSeries() override = default;

  inline auto first() const -> uint64_t {
    return Series<uint64_t, Capacity>::ToObservable().as_blocking().first();
  }

  inline auto last() const -> uint64_t {
    return Series<uint64_t, Capacity>::ToObservable().as_blocking().last();
  }

  inline auto average() const -> uint64_t {
    return Series<uint64_t, Capacity>::ToObservable().as_blocking().average();
  }

  inline auto max() const -> uint64_t {
    return Series<uint64_t, Capacity>::ToObservable().as_blocking().max();
  }

  inline auto min() const -> uint64_t {
    return Series<uint64_t, Capacity>::ToObservable().as_blocking().min();
  }

  explicit operator rx::observable<uint64_t>() const {
    return Series<uint64_t, Capacity>::ToObservable();
  }
};

template <const uint64_t Capacity = 10>
class TimeSeries : public NumericSeries<uint64_t, Capacity> {
 public:
  TimeSeries() = default;
  ~TimeSeries() override = default;

  explicit operator rx::observable<uint64_t>() const {
    return Series<uint64_t, Capacity>::ToObservable();
  }

  friend auto operator<<(std::ostream& stream, const TimeSeries<Capacity>& rhs) -> std::ostream& {
    using namespace units::time;
    stream << "TimeSeries(";
    stream << "size=" << (Capacity) << ", ";
    stream << "avg=" << nanosecond_t(rhs.average()) << ", ";
    stream << "min=" << nanosecond_t(rhs.min()) << ", ";
    stream << "max=" << nanosecond_t(rhs.max());
    stream << ")";
    return stream;
  }
};
}  // namespace prt

#endif  // PRT_SERIES_H
