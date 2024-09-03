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
  typedef uv_idle_t IdleHandle;

  static inline Status
  InitIdle(uv_loop_t* loop, IdleHandle* handle) {
    PRT_ASSERT(loop);
    PRT_ASSERT(handle);
    return Status(uv_idle_init(loop, handle));
  }

  static inline Status
  InitIdle(Loop* loop, IdleHandle* handle) {
    return InitIdle(loop->GetLoop(), handle);
  }

  static inline Status
  StartIdle(IdleHandle* handle, uv_idle_cb cb) {
    PRT_ASSERT(handle);
    return Status(uv_idle_start(handle, cb));
  }

  static inline Status
  StopIdle(IdleHandle* handle) {
    PRT_ASSERT(handle);
    return Status(uv_idle_stop(handle));
  }

  class Idle : public HandleBaseTemplate<IdleHandle>  {
    typedef std::function<void(Idle*)> Callback;
  private:
    Callback callback_;

    static void OnIdle(IdleHandle* handle);

    const Callback& GetCallback() const {
      return callback_;
    }
  public:
    Idle(Loop* loop,
         const Callback& callback,
         const bool start = true);
    ~Idle() override;

    void Start();
    void Stop();
  };
}

#endif //PRT_UV_HANDLE_IDLE_H