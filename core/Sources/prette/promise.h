#ifndef PRT_PROMISE_H
#define PRT_PROMISE_H

#include <exception>
#include <functional>
#include <utility>

#include "prette/common.h"

namespace prt {
template <typename Result>
class Promise {
  DEFINE_DEFAULT_COPYABLE_TYPE(Promise<Result>);

 public:
  using OnSuccessCallback = std::function<void(Result)>;
  using OnErrorCallback = std::function<void(std::exception_ptr)>;
  using OnCompleteCallback = std::function<void()>;

 private:
  OnSuccessCallback on_success_{};
  OnErrorCallback on_error_{};
  OnCompleteCallback on_complete_{};

 public:
  Promise(OnSuccessCallback on_success, OnErrorCallback on_error = nullptr, OnCompleteCallback on_complete = nullptr) :
    on_success_(std::move(on_success)),
    on_error_(std::move(on_error)),
    on_complete_(std::move(on_complete)) {}
  ~Promise() = default;

  auto on_success(Result value) {
    if (on_success_)
      on_success_(value);
  }

  auto on_complete() {
    if (on_complete_)
      on_complete_();
  }

  auto on_error(std::exception_ptr exc) {
    if (on_error_)
      on_error_(exc);
  }
};
}  // namespace prt

#endif  // PRT_PROMISE_H
