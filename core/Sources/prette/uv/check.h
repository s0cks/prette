#ifndef PRT_CHECK_H
#define PRT_CHECK_H

#include <uv.h>

#include "prette/common.h"
#include "prette/uv/handle.h"
#include "prette/uv/loop.h"

namespace prt::uv {
class Check : public CallbackHandleTemplate<uv_check_t> {
  static void OnCheck(uv_check_t* handle);
  DEFINE_NON_COPYABLE_TYPE(Check);

 public:
  Check(Loop& loop, const Callback callback);
  ~Check() override = default;

  void Start();
  void Stop();
};
}  // namespace prt::uv

#endif  // PRT_CHECK_H
