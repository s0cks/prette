#include "prette/os_thread.h"

#include <glog/logging.h>

#include "prette/assertions.h"
#include "prette/relaxed_atomic.h"

namespace prt {
static RelaxedAtomic<ThreadId> kMainThreadId;

void MainThread::SetThreadId(const ThreadId id) {
  kMainThreadId = id;
}

auto MainThread::GetThreadId() -> ThreadId {
  return (ThreadId)kMainThreadId;
}

void MainThread::Init(const ThreadId id) {
  DLOG(INFO) << "initializing main thread on thread: " << id;
  SetThreadId(id);
}

void OSThread::HandleThread(void* data) {
  const auto th = (OSThread*)data;  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  ASSERT(th);
  return th->Run();
}

auto OSThread::Start() -> bool {
  return prt::Start(&id_, name_, &HandleThread, this);
}

auto OSThread::Join() -> bool {
  return prt::Join(id_);
}
}  // namespace prt