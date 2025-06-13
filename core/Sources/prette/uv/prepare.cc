#include "prette/uv/prepare.h"

#include <utility>
#include <uv.h>

#include "prette/common.h"
#include "prette/uv/handle.h"
#include "prette/uv/loop.h"
#include "prette/uv/status.h"

namespace prt::uv {
Prepare::Prepare(Loop& loop, const Callback callback) :
  CallbackHandleTemplate<uv_prepare_t>(std::move(callback)) {
  const uv::Status status = uv_prepare_init(loop, handle_ptr());
  LOG_IF(FATAL, !status) << "uv_prepare_init failed: " << status;
  uv::SetHandleData<uv_prepare_t>(handle_ptr(), this);
}

void Prepare::Start() {
  const uv::Status status = uv_prepare_start(handle_ptr(), &OnPrepare);
  LOG_IF(FATAL, !status) << "uv_prepare_start failed: " << status;
}

void Prepare::Stop() {
  const uv::Status status = uv_prepare_stop(handle_ptr());
  LOG_IF(FATAL, !status) << "uv_prepare_stop failed: " << status;
}

void Prepare::OnPrepare(uv_prepare_t* handle) {
  return Unwrap<Prepare>(handle)->Invoke();
}
}  // namespace prt::uv