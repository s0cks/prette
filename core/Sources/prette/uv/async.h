#ifndef PRT_ASYNC_H
#define PRT_ASYNC_H

#include <uv.h>

#include "prette/common.h"
#include "prette/uv/handle.h"
#include "prette/uv/loop.h"

namespace prt::uv {
class Async : public CallbackHandleTemplate<uv_async_t> {
  static void OnSend(uv_async_t* handle);
  DEFINE_NON_COPYABLE_TYPE(Async);

 public:
  Async(Loop& loop, const Callback on_send);
  ~Async() override;

  void Send();
};
}  // namespace prt::uv

#endif  // PRT_ASYNC_H
