#include "prette/os_thread.h"

#include <glog/logging.h>
#include "prette/relaxed_atomic.h"

namespace prt {
  static RelaxedAtomic<ThreadId> kMainThreadId;

  void MainThread::SetThreadId(const ThreadId id) {
    kMainThreadId = id;
  }

  ThreadId MainThread::GetThreadId() {
    return (ThreadId) kMainThreadId;
  }

  void MainThread::Init(const ThreadId id) {
    DLOG(INFO) << "initializing main thread on thread: " << id;
    SetThreadId(id);
  }
}