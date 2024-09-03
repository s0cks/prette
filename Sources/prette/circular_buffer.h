#ifndef PRT_CIRCULAR_BUFFER_H
#define PRT_CIRCULAR_BUFFER_H

#include <array>
#include <cstdint>
#include <cstdlib>
#include <glog/logging.h>

namespace prt {
  template<typename T, const uint64_t Capacity>
  class CircularBuffer {
  public:
    typedef std::array<T, Capacity> Container;
    typedef typename Container::iterator iterator;
    typedef typename Container::const_iterator const_iterator;
  private:
    std::array<T, Capacity> data_;
    uint64_t head_ = 0;
    uint64_t tail_ = 0;
    bool full_ : 1;
  public:
    CircularBuffer():
      head_(0),
      tail_(0),
      full_(false) {
      DLOG_IF(ERROR, Capacity <= 0) << "allocating CircularBuffer<" << (typeid(T).name()) << "> w/ capacity of " << Capacity;
    }
    ~CircularBuffer() = default;

    iterator begin() {
      return data_.begin();
    }

    const_iterator begin() const {
      return data_.begin();
    }

    iterator end() {
      return data_.end();
    }

    const_iterator end() const {
      return data_.end();
    }

    void put(T item) {
      data_[head_] = item;
      if(full_)
        tail_ = (tail_ + 1) % Capacity;
      head_ = (head_ + 1) % Capacity;
      full_ = head_ == tail_;
    }

    T get() const {
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

    bool empty() const {
      return !full_ && (head_ == tail_);
    }

    bool full() const {
      return full_;
    }

    uint64_t capacity() const {
      return Capacity;
    }

    uint64_t size() const {
      if(full_)
        return Capacity;
      return head_ >= tail_
        ? head_ - tail_
        : Capacity + head_ - tail_;
    }

    T& operator[](const uint64_t idx) const {
      return data_[(tail_ + idx) % Capacity];
    }
  };
}


#endif //PRT_CIRCULAR_BUFFER_H