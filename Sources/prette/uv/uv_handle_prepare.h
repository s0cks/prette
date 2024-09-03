#ifndef PRT_UV_HANDLE_H
#error "Please #include <prette/uv/handle.h> instead."
#endif //PRT_UV_HANDLE_H

#ifndef PRT_UV_HANDLE_PREPARE_H
#define PRT_UV_HANDLE_PREPARE_H

#include <functional>
#include "prette/uv/uv_handle.h"
#include "prette/uv/uv_status.h"

namespace prt::uv {
  typedef uv_prepare_t PrepareHandle;  

  static inline Status
  InitPrepare(uv_loop_t* loop, PrepareHandle* handle) {
    return Status(uv_prepare_init(loop, handle));
  }

  static inline Status
  InitPrepare(Loop* loop, PrepareHandle* handle) {
    return InitPrepare(loop->GetLoop(), handle);
  }

  static inline Status
  StartPrepare(PrepareHandle* handle, uv_prepare_cb cb) {
    return Status(uv_prepare_start(handle, cb));
  }

  static inline Status
  StopPrepare(PrepareHandle* handle) {
    return Status(uv_prepare_stop(handle));
  }

  class Prepare : public HandleBaseTemplate<PrepareHandle> {
  public:
    typedef std::function<void(Prepare*)> Callback;
  private:
    Callback callback_;

    static void OnPrepare(PrepareHandle* handle);

    const Callback& GetCallback() const {
      return callback_;
    }
  public:
    explicit Prepare(Loop* loop,
                     const Callback& callback,
                     const bool start = true);
    ~Prepare() override;

    void Start();
    void Stop();
  };
}

#endif //PRT_UV_HANDLE_PREPARE_H