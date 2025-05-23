#ifndef PRT_SCHEDULER_H
#define PRT_SCHEDULER_H

#include "prette/common.h"
#include "prette/uv/utils.h"

namespace prt {
class Scheduler {
  friend class Engine;

 private:
  uv::Loop* loop_;

  explicit Scheduler(uv::Loop* loop) :
    loop_(loop) {
    ASSERT(loop_);
  }

  auto ScheduleWork(uv::Work* work) -> uv::Work*;

 public:
  ~Scheduler() = default;

  auto GetLoop() const -> uv::Loop* {
    return loop_;
  }

  template <typename W>
  auto Schedule(W* work) -> W* {
    ASSERT(work);
    return ScheduleWork(work);
  }

  template <typename W, typename... Args>
  auto Schedule(Args... args) -> W* {
    return (W*)ScheduleWork(new W(args...));
  }

  auto Schedule(uv::AnyWork::OnWorkCallback on_work, uv::AnyWork::OnWorkFinishedCallback on_finished = nullptr)
      -> uv::AnyWork* {
    return Schedule<uv::AnyWork>(on_work, on_finished);
  }
};

auto GetEngineScheduler() -> Scheduler&;
}  // namespace prt

#endif  // PRT_SCHEDULER_H
