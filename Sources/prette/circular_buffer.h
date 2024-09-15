#ifndef PRT_CIRCULAR_BUFFER_H
#define PRT_CIRCULAR_BUFFER_H

#include <array>
#include <cstdint>
#include <cstdlib>
#include <glog/logging.h>

#include "prette/common.h"

namespace prt {
  template<typename T, const uint64_t Capacity>
  class CircularBuffer {
    DEFINE_NON_COPYABLE_TYPE(CircularBuffer);
  public:
    using Container = std::array<T, Capacity>;
    using iterator = typename Container::iterator;
    using const_iterator = typename Container::const_iterator;
  private:
    std::array<T, Capacity> data_;
    uint64_t head_{};
    uint64_t tail_{};
    bool full_ : 1{};
  public:
    CircularBuffer() = default;
    ~CircularBuffer() = default;

    auto begin() -> iterator {
      return data_.begin();
    }

    auto begin() const -> const_iterator {
      return data_.begin();
    }

    auto end() -> iterator {
      return data_.end();
    }

    auto end() const -> const_iterator {
      return data_.end();
    }

    void put(T item) {
      data_[head_] = item;
      if(full_)
        tail_ = (tail_ + 1) % Capacity;
      head_ = (head_ + 1) % Capacity;
      full_ = head_ == tail_;
    }

    auto get() -> T {
      if(empty())
        return (T)nullptr;
      auto val = data_[tail_];
      full_ = false;
      tail_ = (tail_ + 1) % Capacity;
      return val;
    }

    void reset() {
      head_ = tail_;
      full_ = false;
    }

    auto empty() const -> bool {
      return !full_ && (head_ == tail_);
    }

    auto full() const -> bool {
      return full_;
    }

    auto capacity() const -> uint64_t {
      return Capacity;
    }

    auto size() const -> uint64_t {
      if(full_)
        return Capacity;
      return head_ >= tail_
        ? head_ - tail_
        : Capacity + head_ - tail_;
    }

    auto operator[](const uint64_t idx) const -> T& {
      return data_[(tail_ + idx) % Capacity];
    }
  };
}


#endif //PRT_CIRCULAR_BUFFER_H