#ifndef PRT_UV_HANDLE_H
#error "Please #include <prette/uv/uv_handle.h> instead."
#endif //PRT_UV_HANDLE_H

#ifndef PRT_UV_HANDLE_CHECK_H
#define PRT_UV_HANDLE_CHECK_H

#include <functional>
#include "prette/uv/uv_loop.h"
#include "prette/uv/uv_handle.h"
#include "prette/uv/uv_status.h"

namespace prt::uv {
  typedef uv_check_t CheckHandle;

  static inline Status
  InitCheck(uv_loop_t* loop, CheckHandle* handle) {
    return Status(uv_check_init(loop, handle));
  }

  static inline Status
  InitCheck(Loop* loop, CheckHandle* handle) {
    return InitCheck(loop->GetLoop(), handle);
  }

  static inline Status
  StartCheck(CheckHandle* handle, uv_check_cb cb) {
    return Status(uv_check_start(handle, cb));
  }

  static inline Status
  StopCheck(CheckHandle* handle) {
    return Status(uv_check_stop(handle));
  }

  class Check : public HandleBaseTemplate<uv_check_t> {
    DEFINE_NON_COPYABLE_TYPE(Check);
  public:
    typedef std::function<void(Check*)> Callback;
  private:
    Callback callback_;

    static void OnCheck(CheckHandle* handle);

    const Callback& GetCallback() const {
      return callback_;
    }
  public:
    explicit Check(Loop* loop,
                   const Callback& callback,
                   const bool start = true);
    ~Check() override;

    void Start();
    void Stop();
  };
}

#endif //PRT_UV_HANDLE_CHECK_H