#include "prette/uv/loop.h"

#include <uv.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/uv/status.h"
#include "prette/uv/utils.h"

namespace prt::uv {
Loop::Loop() :
  handle_() {
  const uv::Status status = uv_loop_init(handle_ptr());
  LOG_IF(FATAL, !status) << "uv_loop_init failed: " << status;
  uv_loop_set_data(handle_ptr(), this);
}

Loop::~Loop() {
  uv_loop_close(handle_ptr());
}

void Loop::Run(const RunMode mode) {
  const uv::Status status = uv_run(handle_ptr(), static_cast<uv_run_mode>(mode));
  LOG_IF(FATAL, !status) << "uv_run failed: " << status;
}

void Loop::Stop() {
  uv_stop(handle_ptr());
}

auto Loop::Queue(Work* work) -> Status {
  ASSERT(work);
  return work->Submit(this);
}

auto Loop::Now() const -> uint64_t {
  return uv_now(&handle());
}

void Loop::UpdateTime() {
  uv_update_time(handle_ptr());
}
}  // namespace prt::uv