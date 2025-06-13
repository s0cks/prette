#ifndef PRT_IDLE_H
#define PRT_IDLE_H

#include <uv.h>

#include "prette/common.h"
#include "prette/uv/handle.h"
#include "prette/uv/loop.h"

namespace prt::uv {
class Idle : public CallbackHandleTemplate<uv_idle_t> {
  static void OnIdle(uv_idle_t* handle);
  DEFINE_NON_COPYABLE_TYPE(Idle);

 public:
  Idle(Loop& loop, const Callback callback);
  ~Idle() override = default;

  void Start();
  void Stop();
};
}  // namespace prt::uv

#endif  // PRT_IDLE_H
