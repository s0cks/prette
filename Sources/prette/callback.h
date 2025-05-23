#ifndef PRT_CALLBACK_H
#define PRT_CALLBACK_H

#include <functional>
#include <type_traits>
#include <utility>

#include "prette/common.h"

namespace prt {
template <typename R, typename... Args>
class Callback {
 public:
  using CallbackType = Callback<R, Args...>;
  using FunctionType = std::function<R(Args...)>;

  class Iterator {
   private:
    CallbackType* current_;

   public:
    explicit Iterator(CallbackType* head) :
      current_(head) {}
    ~Iterator() = default;

    auto HasNext() const -> bool {
      return current_ != nullptr;
    }

    auto Next() -> CallbackType* {
      const auto next = current_;
      current_ = next->GetNext();
      return next;
    }
  };

 private:
  FunctionType func_{};
  CallbackType* next_ = nullptr;
  CallbackType* previous_ = nullptr;

 public:
  explicit Callback(FunctionType func) :
    func_(std::move(func)) {}
  ~Callback() = default;

  auto GetNext() const -> CallbackType* {
    return next_;
  }

  inline auto HasNext() const -> bool {
    return GetNext() != nullptr;
  }

  void SetNext(CallbackType* rhs) {
    ASSERT(rhs);
    next_ = rhs;
  }

  auto GetPrevious() const -> CallbackType* {
    return previous_;
  }

  inline auto HasPrevious() const -> bool {
    return GetPrevious() != nullptr;
  }

  void SetPrevious(CallbackType* rhs) {
    ASSERT(rhs);
    previous_ = rhs;
  }

  auto Apply(Args... args) -> R {
    return func_(args...);
  }

 public:
  static inline void Append(CallbackType** list, CallbackType* value) {
    if ((*list) == nullptr) {
      (*list) = value;
      return;
    }
    auto last = (*list);
    while (last->HasNext())
      last = last->GetNext();
    last->SetNext(value);
    value->SetPrevious(last);
  }

  static inline void Remove(CallbackType** list, CallbackType* value) {
    NOT_IMPLEMENTED(ERROR);
  }

  static inline auto ApplyAll(CallbackType* list, Args... args) -> std::enable_if_t<std::is_void_v<R>> {
    Iterator iter(list);
    while (iter.HasNext())
      iter.Next()->Apply(args...);
  }
};
}  // namespace prt

#endif  // PRT_CALLBACK_H
