#ifndef PRT_RELAXED_ATOMIC_H
#define PRT_RELAXED_ATOMIC_H

#include <atomic>
#include <iostream>

namespace prt {
template <typename T>
class RelaxedAtomic {
 protected:
  std::atomic<T> value_{};

 public:
  RelaxedAtomic() :
    value_() {}
  constexpr RelaxedAtomic(const T& value) :
    value_(value) {}
  RelaxedAtomic(const RelaxedAtomic<T>& rhs) :
    value_((T)rhs) {}
  ~RelaxedAtomic() = default;

  auto fetch_add(T arg, std::memory_order order = std::memory_order_relaxed) -> T {
    return value_.fetch_add(arg, order);
  }

  auto fetch_sub(T arg, std::memory_order order = std::memory_order_relaxed) -> T {
    return value_.fetch_sub(arg, order);
  }

  auto load(std::memory_order order = std::memory_order_relaxed) const -> T {
    return value_.load(order);
  }

  void store(T arg, std::memory_order order = std::memory_order_relaxed) {
    value_.store(arg, order);
  }

  auto compare_exchange_weak(T& expected, T desired, std::memory_order order = std::memory_order_relaxed) -> bool {
    return value_.compare_exchange_weak(expected, desired, order, order);
  }

  auto compare_exchange_strong(T& expected, T desired, std::memory_order order = std::memory_order_relaxed) -> bool {
    return value_.compare_exchange_strong(expected, desired, order, order);
  }

  explicit operator T() const {
    return load();
  }

  auto operator=(T arg) -> RelaxedAtomic<T>& {
    store(arg);
    return *this;
  }

  auto operator=(const RelaxedAtomic& arg) -> RelaxedAtomic<T>& {
    T loaded = (T)arg;
    store(loaded);
    return *this;
  }

  auto operator+=(T arg) -> T {
    return fetch_add(arg) + arg;
  }

  auto operator-=(T arg) -> T {
    return fetch_sub(arg) - arg;
  }

  friend auto operator==(const RelaxedAtomic<T>& lhs, const T& rhs) -> bool {
    return ((T)lhs) == rhs;
  }

  friend auto operator!=(const RelaxedAtomic<T>& lhs, const T& rhs) -> bool {
    return ((T)lhs) != rhs;
  }

  friend auto operator<(const RelaxedAtomic<T>& lhs, const T& rhs) -> bool {
    return ((T)lhs) < rhs;
  }

  friend auto operator>(const RelaxedAtomic<T>& lhs, const T& rhs) -> bool {
    return ((T)lhs) > rhs;
  }

  friend auto operator<<(std::ostream& stream, const RelaxedAtomic<T>& val) -> std::ostream& {
    return stream << ((T)val);
  }
};
}  // namespace prt

#endif  // PRT_RELAXED_ATOMIC_H
