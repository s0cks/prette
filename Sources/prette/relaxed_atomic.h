#ifndef PRT_RELAXED_ATOMIC_H
#define PRT_RELAXED_ATOMIC_H

#include <atomic>

namespace prt {
  template<typename T>
  class RelaxedAtomic {
  protected:
    std::atomic<T> value_;
  public:
    RelaxedAtomic():
      value_() {
    }
    constexpr RelaxedAtomic(const T& value):
      value_(value) {
    }
    RelaxedAtomic(const RelaxedAtomic<T>& rhs):
      value_((T) rhs) {
    }
    ~RelaxedAtomic() = default;

    T fetch_add(T arg, std::memory_order order=std::memory_order_relaxed){
      return value_.fetch_add(arg, order);
    }

    T fetch_sub(T arg, std::memory_order order=std::memory_order_relaxed){
      return value_.fetch_sub(arg, order);
    }

    T load(std::memory_order order=std::memory_order_relaxed) const{
      return value_.load(order);
    }

    void store(T arg, std::memory_order order=std::memory_order_relaxed){
      value_.store(arg, order);
    }

    bool compare_exchange_weak(T& expected, T desired, std::memory_order order=std::memory_order_relaxed){
      return value_.compare_exchange_weak(expected, desired, order, order);
    }

    bool compare_exchange_strong(T& expected, T desired, std::memory_order order=std::memory_order_relaxed){
      return value_.compare_exchange_strong(expected, desired, order, order);
    }

    explicit operator T() const{
      return load();
    }

    T operator=(T arg){ // NOLINT(misc-unconventional-assign-operator)
      store(arg);
      return arg;
    }

    T operator=(const RelaxedAtomic& arg){ // NOLINT(misc-unconventional-assign-operator)
      T loaded = (T)arg;
      store(loaded);
      return loaded;
    }

    T operator+=(T arg){
      return fetch_add(arg) + arg;
    }

    T operator-=(T arg){
      return fetch_sub(arg) - arg;
    }

    friend bool operator==(const RelaxedAtomic<T>& lhs, const T& rhs){
      return ((T)lhs) == rhs;
    }

    friend bool operator!=(const RelaxedAtomic<T>& lhs, const T& rhs){
      return ((T)lhs) != rhs;
    }

    friend bool operator<(const RelaxedAtomic<T>& lhs, const T& rhs){
      return ((T)lhs) < rhs;
    }

    friend bool operator>(const RelaxedAtomic<T>& lhs, const T& rhs){
      return ((T)lhs) > rhs;
    }

    friend std::ostream& operator<<(std::ostream& stream, const RelaxedAtomic<T>& val){
      return stream << ((T)val);
    }
  };
}

#endif //PRT_RELAXED_ATOMIC_H