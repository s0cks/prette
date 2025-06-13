#include "prette/uv/check.h"

#include <utility>
#include <uv.h>

#include "prette/common.h"
#include "prette/uv/handle.h"
#include "prette/uv/loop.h"
#include "prette/uv/status.h"

namespace prt::uv {
Check::Check(Loop& loop, const Callback callback) :
  CallbackHandleTemplate<uv_check_t>(std::move(callback)) {
  const uv::Status status = uv_check_init(loop, handle_ptr());
  LOG_IF(FATAL, !status) << "uv_check_init failed: " << status;
  uv::SetHandleData<uv_check_t>(handle_ptr(), this);
}

void Check::Start() {
  const uv::Status status = uv_check_start(handle_ptr(), &OnCheck);
  LOG_IF(FATAL, !status) << "uv_check_start failed: " << status;
}

void Check::Stop() {
  const uv::Status status = uv_check_stop(handle_ptr());
  LOG_IF(FATAL, !status) << "uv_check_stop failed: " << status;
}

void Check::OnCheck(uv_check_t* handle) {
  return Unwrap<Check>(handle)->Invoke();
}
}  // namespace prt::uv