#ifndef PRT_COUNTER_H
#define PRT_COUNTER_H

#include "prette/uv/utils.h"
#include "prette/relaxed_atomic.h"

namespace prt {
  template<typename T>
  class Counter {
  private:
    RelaxedAtomic<T> value_;
  public:
    explicit Counter(const T init_value = 0):
      value_(init_value) {
    }
    Counter(const Counter<T>&& rhs) = default;
    Counter(const Counter<T>& rhs) = default;
    virtual ~Counter() = default;

    auto value() const -> T {
      return (T) value_;
    }

    auto operator=(Counter<T>&& rhs) -> Counter<T>& = default;
    auto operator=(const Counter<T>& rhs) -> Counter<T>& = default;

    operator T () const {
      return value();
    }

    auto operator=(const T& rhs) -> Counter<T>& {
      value_ = rhs;
      return *this;    
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

    friend auto operator<<(std::ostream& stream, const Counter<T>& rhs) -> std::ostream& {
      return stream << (T)rhs;
    }
  };

  template<typename T>
  class PerSecondCounter : public Counter<T> {
  private:
    uint64_t last_second_;
    T per_second_;

    inline auto
    SecondHasElapsed(const uint64_t ts) -> bool {
      return (ts - last_second_) >= NSEC_PER_SEC;
    }
  public:
    explicit PerSecondCounter(const T init_value = 0):
      Counter<T>(init_value),
      last_second_(uv_hrtime()),
      per_second_(0) {
    }
    PerSecondCounter(const PerSecondCounter<T>&& rhs) = default;
    PerSecondCounter(const PerSecondCounter<T>& rhs) = default;
    ~PerSecondCounter() override = default;

    auto per_sec() const -> T {
      return per_second_;
    }

    void Increment(const T value = 1, const uint64_t ts = uv_hrtime()) {
      Counter<T>::operator+=(value);
      if(SecondHasElapsed(ts)) {
        per_second_ = (T) Counter<T>::value();
        last_second_ = ts;
        Counter<T>::operator=(0);
      }
    }

    void Decrement(const T value = 1, const uint64_t ts = uv_hrtime()) {
      Counter<T>::operator-=(value);
      if(SecondHasElapsed(ts)) {
        per_second_ = (T) Counter<T>::value();
        last_second_ = ts;
        Counter<T>::operator=(0);
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
      return (T) Counter<T>::value();
    }

    friend auto operator<<(std::ostream& stream, const PerSecondCounter<T>& rhs) -> std::ostream& {
      return stream << (T)rhs << " (" << rhs.per_sec() << "/s)";
    }

    auto operator=(const PerSecondCounter<T>& rhs) -> PerSecondCounter<T>& = default;
    auto operator=(PerSecondCounter<T>&& rhs) -> PerSecondCounter<T>& = default;
  };
}

#endif //PRT_COUNTER_H