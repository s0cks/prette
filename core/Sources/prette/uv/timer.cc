#include "prette/uv/timer.h"

#include <cstdint>
#include <utility>
#include <uv.h>

#include "prette/common.h"
#include "prette/uv/handle.h"
#include "prette/uv/loop.h"
#include "prette/uv/status.h"

namespace prt::uv {
Timer::Timer(Loop& loop, const Callback callback) :
  CallbackHandleTemplate<uv_timer_t>(std::move(callback)) {
  const uv::Status status = uv_timer_init(loop, handle_ptr());
  LOG_IF(FATAL, !status) << "uv_timer_init failed: " << status;
  uv::SetHandleData<uv_timer_t>(handle_ptr(), this);
}

void Timer::SetRepeat(const uint64_t rhs) {
  uv_timer_set_repeat(handle_ptr(), rhs);
}

void Timer::Again() {
  const uv::Status status = uv_timer_again(handle_ptr());
  LOG_IF(FATAL, !status) << "uv_timer_again failed: " << status;
}

void Timer::Start(const uint64_t timeout, const uint64_t repeat) {
  const uv::Status status = uv_timer_start(handle_ptr(), &OnTick, timeout, repeat);
  LOG_IF(FATAL, !status) << "uv_timer_start failed: " << status;
}

void Timer::Stop() {
  const uv::Status status = uv_timer_stop(handle_ptr());
  LOG_IF(FATAL, !status) << "uv_timer_stop failed: " << status;
}

void Timer::OnTick(uv_timer_t* handle) {
  return Unwrap<Timer>(handle)->Invoke();
}
}  // namespace prt::uv