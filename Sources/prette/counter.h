#ifndef PRT_COUNTER_H
#define PRT_COUNTER_H

#include "prette/uv/utils.h"
#include "prette/relaxed_atomic.h"

namespace prt {
  template<typename T>
  class Counter {
  protected:
    RelaxedAtomic<T> value_;
  public:
    explicit Counter(const T init_value = 0):
      value_(init_value) {
    }
    Counter(const Counter<T>& rhs) = default;
    virtual ~Counter() = default;

    Counter<T>& operator=(const Counter<T>& rhs) = default;

    operator T () const {
      return (T) value_;
    }

    void operator++() {
      value_ += 1;
    }

    void operator+=(const T& rhs) {
      value_ += rhs;
    }

    void operator--() {
      value_ -= 1;
    }

    void operator-=(const T& rhs) {
      value_ -= rhs;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Counter<T>& rhs) {
      return stream << (T)rhs;
    }
  };

  template<typename T>
  class PerSecondCounter : public Counter<T> {
  protected:
    uint64_t last_second_;
    T per_second_;

    inline bool
    SecondHasElapsed(const uint64_t ts) {
      return (ts - last_second_) >= NSEC_PER_SEC;
    }
  public:
    explicit PerSecondCounter(const T init_value = 0):
      Counter<T>(init_value),
      last_second_(uv_hrtime()),
      per_second_(0) {
    }
    ~PerSecondCounter() override = default;

    T per_sec() const {
      return per_second_;
    }

    void Increment(const T value = 1, const uint64_t ts = uv_hrtime()) {
      Counter<T>::value_ += value;
      if(SecondHasElapsed(ts)) {
        per_second_ = (T) Counter<T>::value_;
        last_second_ = ts;
        Counter<T>::value_ = 0;
      }
    }

    void Decrement(const T value = 1, const uint64_t ts = uv_hrtime()) {
      Counter<T>::value_ -= value;
      if(SecondHasElapsed(ts)) {
        per_second_ = (T) Counter<T>::value_;
        last_second_ = ts;
        Counter<T>::value_ = 0;
      }
    }

    void operator++() {
      return Increment();
    }

    void operator+=(const T& rhs) {
      return Increment(rhs);
    }

    void operator--() {
      return Decrement();
    }

    void operator-=(const T& rhs) {
      return Decrement(rhs);
    }

    operator T () const {
      return (T) Counter<T>::value_;
    }

    friend std::ostream& operator<<(std::ostream& stream, const PerSecondCounter<T>& rhs) {
      return stream << (T)rhs << " (" << rhs.per_sec() << "/s)";
    }
  };
}

#endif //PRT_COUNTER_H