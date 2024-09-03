#ifndef PRT_SERIES_H
#define PRT_SERIES_H

#include <units.h>
#include <functional>
#include <algorithm>

#include "prette/rx.h"
#include "prette/circular_buffer.h"

namespace prt {
  template<typename T, const uint64_t Capacity>
  class Series {
  protected:
    CircularBuffer<T, Capacity> data_;

    Series():
      data_() {
    }
  public:
    virtual ~Series() = default;

    void Append(const T value) {
      data_.put(value);
    }

    T* begin() {
      return data_.begin();
    }

    T* begin() const {
      return data_.begin();
    }

    T* end() {
      return data_.end();
    }

    T* end() const {
      return data_.end();
    }

    uint64_t count() const {
      return ToObservable()
        .as_blocking()
        .count();
    }

    rx::observable<T> ToObservable() const {
      return rx::observable<>::create<T>([this](rx::subscriber<T> s) {
        for(const auto& value : data_) {
          s.on_next(value);
        }
        s.on_completed();
      });
    }

    explicit operator rx::observable<T> () const {
      return ToObservable();
    }
  };

  template<typename T, const uint64_t Capacity = 10>
  class NumericSeries : public Series<uint64_t, Capacity> {
  public:
    NumericSeries() = default;
    ~NumericSeries() override = default;

    inline uint64_t first() const {
      return Series<uint64_t, Capacity>::ToObservable()
        .as_blocking()
        .first();
    }

    inline uint64_t last() const {
      return Series<uint64_t, Capacity>::ToObservable()
        .as_blocking()
        .last();
    }

    inline uint64_t average() const {
      return Series<uint64_t, Capacity>::ToObservable()
        .as_blocking()
        .average();
    }

    inline uint64_t max() const {
      return Series<uint64_t, Capacity>::ToObservable()
        .as_blocking()
        .max();
    }

    inline uint64_t min() const {
      return Series<uint64_t, Capacity>::ToObservable()
        .as_blocking()
        .min();
    }

    explicit operator rx::observable<uint64_t> () const {
      return Series<uint64_t, Capacity>::ToObservable();
    }
  };

  template<const uint64_t Capacity = 10>
  class TimeSeries : public NumericSeries<uint64_t, Capacity> {
  public:
    TimeSeries() = default;
    ~TimeSeries() override = default;

    explicit operator rx::observable<uint64_t> () const {
      return Series<uint64_t, Capacity>::ToObservable();
    }

    friend std::ostream& operator<<(std::ostream& stream, const TimeSeries<Capacity>& rhs) {
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
}

#endif //PRT_SERIES_H