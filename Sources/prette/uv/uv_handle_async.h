#ifndef PRT_UV_HANDLE_H
#error "Please #include <prette/uv/handle.h> instead."
#endif //PRT_UV_HANDLE_H

#ifndef PRT_UV_HANDLE_ASYNC_H
#define PRT_UV_HANDLE_ASYNC_H

#include "prette/common.h"
#include "prette/uv/uv_loop.h"
#include "prette/uv/uv_handle.h"
#include "prette/uv/uv_status.h"

namespace prt::uv {
  static inline Status
  InitAsync(uv_loop_t* loop, uv_async_t* handle, uv_async_cb cb) {
    PRT_ASSERT(loop);
    PRT_ASSERT(handle);
    return Status(uv_async_init(loop, handle, cb));
  }

  static inline Status
  InitAsync(Loop* loop, uv_async_t* handle, uv_async_cb cb) {
    PRT_ASSERT(loop);
    PRT_ASSERT(handle);
    return InitAsync(loop->GetLoop(), handle, cb);
  }

  static inline Status
  AsyncSend(uv_async_t* handle) {
    PRT_ASSERT(handle);
    return Status(uv_async_send(handle));
  }

  class AsyncHandleBase : public HandleBase {
  protected:
    uv_async_t handle_;

    AsyncHandleBase(uv_loop_t* loop, uv_async_cb callback):
      handle_() {
      const auto err = uv_async_init(loop, &handle_, callback);
      LOG_IF(ERROR, err != UV_OK) << "uv_async_init failed: " << uv_strerror(err);
    }
  public:
    virtual ~AsyncHandleBase() = default;//TODO: remove async handle from loop?

    virtual void Call() {
      const auto err = uv_async_send(&handle_);
      LOG_IF(ERROR, err != UV_OK) << "uv_async_send failed: " << uv_strerror(err);
    }
  };

  template<typename D>
  class AsyncHandle : public AsyncHandleBase {
  public:
    typedef std::function<void(D*)> Callback;
  private:
    static inline void
    OnCall(uv_async_t* handle) {
      const auto async = GetHandleData<AsyncHandle>(handle);
      async->callback_(async->GetData());
    }
  protected:
    Callback callback_;
    uword data_;
  public:
    AsyncHandle(uv_loop_t* loop,
                const Callback& callback,
                const uword data = 0):
      AsyncHandleBase(loop, &OnCall),
      callback_(callback),
      data_((uword) data) {
    }
    AsyncHandle(uv_loop_t* loop,
                const Callback& callback,
                const D* data = 0):
      AsyncHandle(loop, callback, (uword) data) {
    }
    ~AsyncHandle() = default; //TODO: remove async handle from loop?

    D* GetData() const {
      return (D*) data_;
    }

    void operator() () {
      return Call();
    }
  };
}

#endif //PRT_UV_HANDLE_ASYNC_H