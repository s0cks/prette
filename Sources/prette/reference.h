#ifndef PRT_REFERENCE_H
#define PRT_REFERENCE_H

#include "prette/common.h"
namespace prt {
template <class T>
class Ref {
  DEFINE_DEFAULT_COPYABLE_TYPE(Ref<T>);

 private:
  T* value_ = nullptr;

 public:
  Ref() = default;
  explicit Ref(const T* value) :
    value_(value) {}
  ~Ref();

  auto get() const -> T* {
    return value_;
  }

  inline auto exists() const -> bool {
    return get() != nullptr;
  }

  operator T*() const {
    return get();
  }

  auto operator->() const -> T* {
    return get();
  }
};
}  // namespace prt

#endif  // PRT_REFERENCE_H
