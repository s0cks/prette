#include "prette/scheduler.h"

#include <glog/logging.h>

#include "prette/common.h"
#include "prette/engine.h"
#include "prette/uv/utils.h"

namespace prt {
auto Scheduler::ScheduleWork(uv::Work* work) -> uv::Work* {
  const auto status = GetLoop()->Queue(work);
  if (!status) {
    LOG(ERROR) << "failed to schedule " << work->ToString() << ": " << status;
    return nullptr;
  }
  DLOG(INFO) << work->ToString() << " scheduled!";
  return work;
}

auto GetEngineScheduler() -> Scheduler& {
  return GetEngine()->GetScheduler();
}
}  // namespace prt