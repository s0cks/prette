#ifndef PRT_THREAD_LOCAL_H
#define PRT_THREAD_LOCAL_H

#include <glog/logging.h>
#include "prette/os_thread.h"

namespace prt {
  template<typename T>
  class ThreadLocal {
    DEFINE_NON_COPYABLE_TYPE(ThreadLocal<T>);
  private:
    ThreadLocalKey key_{};
  public:
    ThreadLocal() {
      LOG_IF(FATAL, !InitializeThreadLocal(key_));
    }
    virtual ~ThreadLocal() = default;

    auto GetKey() const -> const ThreadLocalKey& {
      return key_;
    }

    virtual auto Set(T* value) const -> bool {
      return SetCurrentThreadLocal(GetKey(), (void*) value);
    }

    virtual auto Get() const -> T* {
      return (T*) GetCurrentThreadLocal(GetKey());
    }

    auto Has() const -> bool {
      return Get() != nullptr;
    }

    operator bool () const {
      return Has();
    }

    friend auto operator<<(std::ostream& stream, const ThreadLocal<T>& rhs) -> std::ostream& {
      return stream << *rhs.Get();
    }
  };

  template<typename T>
  class LazyThreadLocal : public ThreadLocal<T> {
    DEFINE_NON_COPYABLE_TYPE(LazyThreadLocal<T>);
  public:
    using Supplier=std::function<T*()>;
  private:
    static inline auto
    CreateDefaultSupplier() -> Supplier {
      return []() {
        return new T();
      };
    }
  private:
    Supplier supplier_;

    inline auto Supply() const -> T* {
      return supplier_();
    }
  public:
    explicit LazyThreadLocal(const Supplier& supplier = CreateDefaultSupplier()):
      supplier_(supplier) {
    }
    virtual ~LazyThreadLocal() = default;

    auto GetSupplier() const -> const Supplier& {
      return supplier_;
    }

    auto Get() const -> T* override {
      const auto value = ThreadLocal<T>::Get();
      if(value)
        return value;
      const auto supplied = Supply();
      LOG_IF(FATAL, !supplied) << "failed to supply value for ThreadLocal.";
      LOG_IF(FATAL, !ThreadLocal<T>::Set(supplied)) << "failed to set ThreadLocal value.";
      return supplied;
    }
  };
}

#endif //PRT_THREAD_LOCAL_H