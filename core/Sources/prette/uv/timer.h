#ifndef PRT_TIMER_H
#define PRT_TIMER_H

#include <cstdint>
#include <uv.h>

#include "prette/common.h"
#include "prette/uv/handle.h"
#include "prette/uv/loop.h"

namespace prt::uv {
class Timer : public CallbackHandleTemplate<uv_timer_t> {
  static void OnTick(uv_timer_t* handle);
  DEFINE_NON_COPYABLE_TYPE(Timer);

 public:
  Timer(Loop& loop, const Callback callback);
  ~Timer() override = default;

  auto GetRepeat() const -> uint64_t {
    return uv_timer_get_repeat(&handle());
  }

  auto GetDueIn() const -> uint64_t {
    return uv_timer_get_due_in(&handle());
  }

  void SetRepeat(const uint64_t rhs);
  void Start(const uint64_t timeout, const uint64_t repeat);
  void Stop();
  void Again();
};
}  // namespace prt::uv

#endif  // PRT_TIMER_H
