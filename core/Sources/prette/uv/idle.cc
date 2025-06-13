#include "prette/uv/idle.h"

#include <utility>
#include <uv.h>

#include "prette/common.h"
#include "prette/uv/handle.h"
#include "prette/uv/loop.h"
#include "prette/uv/status.h"

namespace prt::uv {
Idle::Idle(Loop& loop, const Callback callback) :
  CallbackHandleTemplate<uv_idle_t>(std::move(callback)) {
  const uv::Status status = uv_idle_init(loop, handle_ptr());
  LOG_IF(FATAL, !status) << "uv_idle_init failed: " << status;
  uv::SetHandleData<uv_idle_t>(handle_ptr(), this);
}

void Idle::Start() {
  const uv::Status status = uv_idle_start(handle_ptr(), &OnIdle);
  LOG_IF(FATAL, !status) << "uv_idle_start failed: " << status;
}

void Idle::Stop() {
  const uv::Status status = uv_idle_stop(handle_ptr());
  LOG_IF(FATAL, !status) << "uv_idle_stop failed: " << status;
}

void Idle::OnIdle(uv_idle_t* handle) {
  return Unwrap<Idle>(handle)->Invoke();
}
}  // namespace prt::uv