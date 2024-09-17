#ifndef PRT_UV_HANDLE_H
#error "Please #include <prette/uv/uv_handle.h> instead."
#endif //PRT_UV_HANDLE_H

#ifndef PRT_UV_HANDLE_IDLE_H
#define PRT_UV_HANDLE_IDLE_H

#include <functional>
#include "prette/uv/uv_loop.h"
#include "prette/uv/uv_handle.h"
#include "prette/uv/uv_status.h"

namespace prt::uv {
  using IdleHandle = uv_idle_t;

  static inline auto
  InitIdle(uv_loop_t* loop, IdleHandle* handle) -> Status {
    PRT_ASSERT(loop);
    PRT_ASSERT(handle);
    return {uv_idle_init(loop, handle)};
  }

  static inline auto
  InitIdle(Loop* loop, IdleHandle* handle) -> Status {
    return InitIdle(loop->GetLoop(), handle);
  }

  static inline auto
  StartIdle(IdleHandle* handle, uv_idle_cb cb) -> Status {
    PRT_ASSERT(handle);
    return {uv_idle_start(handle, cb)};
  }

  static inline auto
  StopIdle(IdleHandle* handle) -> Status {
    PRT_ASSERT(handle);
    return {uv_idle_stop(handle)};
  }

  class Idle : public HandleBaseTemplate<IdleHandle>  {
    using Callback = std::function<void (Idle *)>;
  private:
    Callback callback_;

    static void OnIdle(IdleHandle* handle);

    auto GetCallback() const -> const Callback& {
      return callback_;
    }
  public:
    Idle(Loop* loop,
         Callback callback,
         const bool start = true);
    ~Idle() override;

    void Start();
    void Stop();
  };
}

#endif //PRT_UV_HANDLE_IDLE_H