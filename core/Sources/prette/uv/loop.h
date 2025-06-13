#ifndef PRT_LOOP_H
#define PRT_LOOP_H

#include <cstdint>
#include <ostream>
#include <uv.h>

#include "prette/uv/status.h"

namespace prt::uv {
#define FOR_EACH_UV_RUN_MODE(V) \
  V(Default)                    \
  V(NoWait)                     \
  V(Once)

enum RunMode {
  kRunDefault = UV_RUN_DEFAULT,
  kRunNoWait = UV_RUN_NOWAIT,
  kRunOnce = UV_RUN_ONCE,
};

static inline auto operator<<(std::ostream& stream, const RunMode& rhs) -> std::ostream& {
  switch (rhs) {
    default:
#define DEFINE_TO_STRING(Name) \
  case RunMode::kRun##Name:    \
    return stream << #Name;
      FOR_EACH_UV_RUN_MODE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
  }
}

class Work;
class Loop {
  friend class Idle;
  friend class Check;
  friend class Prepare;

 public:
  using Handle = uv_loop_t;

 private:
  Handle handle_;

  inline auto handle_ptr() -> Handle* {
    return &handle_;
  }

 public:
  explicit Loop();
  ~Loop();

  auto handle() const -> const uv_loop_t& {
    return handle_;
  }

  auto Now() const -> uint64_t;
  void UpdateTime();
  void Stop();
  void Run(const RunMode mode);
  auto Queue(uv::Work* work) -> uv::Status;

#define DEFINE_RUN_WITH_MODE(Name)   \
  inline void Run##Name() {          \
    return Run(RunMode::kRun##Name); \
  }
  FOR_EACH_UV_RUN_MODE(DEFINE_RUN_WITH_MODE);
#undef DEFINE_RUN_WITH_MODE

  operator Handle*() {
    return handle_ptr();
  }

 public:
  static inline auto Unwrap(uv_loop_t* handle) -> Loop* {
    return (Loop*)uv_loop_get_data(handle);
  }
};
}  // namespace prt::uv

#endif  // PRT_LOOP_H
