#ifndef PRT_THREAD_LOCAL_H
#define PRT_THREAD_LOCAL_H

#include <glog/logging.h>
#include "prette/os_thread.h"

namespace prt {
  template<typename T>
  class ThreadLocal {
  protected:
    ThreadLocalKey key_;
  public:
    ThreadLocal():
      key_() {
        LOG_IF(FATAL, !InitializeThreadLocal(key_));  
    }
    virtual ~ThreadLocal() = default;

    bool Set(T* value) {
      return SetCurrentThreadLocal(key_, reinterpret_cast<void*>(value));
    }

    T* Get() const {
      return reinterpret_cast<T*>(GetCurrentThreadLocal(key_));
    }

    operator bool () const {
      return Get() != nullptr;
    }
  };

  template<typename T>
  class LazyThreadLocal {
  public:
    typedef std::function<T*()> Supplier;
  protected:
    ThreadLocalKey local_;
    Supplier supplier_;

    inline bool SetLocal(const T* value) const {
      return SetCurrentThreadLocal(local_, (const void*) value);
    }

    inline T* GetLocal() const {
      return (T*) GetCurrentThreadLocal(local_);
    }

    inline T* Supply() const {
      return supplier_();
    }
  public:
    explicit LazyThreadLocal(Supplier supplier):
      local_(),
      supplier_(supplier) {
    }
    virtual ~LazyThreadLocal() = default;

    T* Get() const {
      auto value = GetLocal();
      if(value == nullptr)
        SetLocal(value = Supply());
      return value;
    }
  };
}

#endif //PRT_THREAD_LOCAL_H