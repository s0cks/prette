#include "prette/uv/utils.h"

#include <uv.h>

#include "prette/assertions.h"
#include "prette/to_string.h"
#include "prette/uv/loop.h"
#include "prette/uv/status.h"

namespace prt::uv {
void AnyWork::OnWork(uv_work_t* handle) {
  const auto work = Unwrap(handle);
  ASSERT(work);
  return work->on_work_(work);
}

void AnyWork::OnWorkFinished(uv_work_t* handle, const int status) {
  const auto work = Unwrap(handle);
  ASSERT(work);
  work->on_finished_(work, status);
  work->SetFinished();
}

auto AnyWork::Submit(Loop* loop) -> Status {
  ASSERT(loop);
  return uv_queue_work(*loop, handle_ptr(), OnWork, OnWorkFinished);
}

auto AnyWork::ToString() const -> std::string {
  return ToStringHelper<AnyWork>{};
}
}  // namespace prt::uv