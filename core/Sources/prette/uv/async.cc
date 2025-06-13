#include "prette/uv/async.h"

#include <utility>
#include <uv.h>

#include "prette/common.h"
#include "prette/uv/handle.h"
#include "prette/uv/loop.h"
#include "prette/uv/status.h"

namespace prt::uv {
Async::Async(Loop& loop, const Callback on_send) :
  CallbackHandleTemplate<uv_async_t>(std::move(on_send)) {
  const uv::Status status = uv_async_init(loop, handle_ptr(), &OnSend);
  LOG_IF(FATAL, !status) << "uv_async_init failed: " << status;
}

Async::~Async() = default;

void Async::OnSend(uv_async_t* handle) {
  return Unwrap<Async>(handle)->Invoke();
}

void Async::Send() {
  const uv::Status status = uv_async_send(handle_ptr());
  LOG_IF(FATAL, !status) << "uv_async_send failed: " << status;
}
}  // namespace prt::uv