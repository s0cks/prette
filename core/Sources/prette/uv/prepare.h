#ifndef PRT_PREPARE_H
#define PRT_PREPARE_H

#include <uv.h>

#include "prette/common.h"
#include "prette/uv/handle.h"
#include "prette/uv/loop.h"

namespace prt::uv {
class Prepare : public CallbackHandleTemplate<uv_prepare_t> {
  static void OnPrepare(uv_prepare_t* handle);
  DEFINE_NON_COPYABLE_TYPE(Prepare);

 public:
  Prepare(Loop& loop, const Callback callback);
  ~Prepare() override = default;

  void Start();
  void Stop();
};
}  // namespace prt::uv

#endif  // PRT_PREPARE_H
