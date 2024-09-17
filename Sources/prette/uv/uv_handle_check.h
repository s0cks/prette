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
  using CheckHandle = uv_check_t;

  static inline auto
  InitCheck(uv_loop_t* loop, CheckHandle* handle) -> Status {
    return {uv_check_init(loop, handle)};
  }

  static inline Status
  InitCheck(Loop* loop, CheckHandle* handle) {
    return InitCheck(loop->GetLoop(), handle);
  }

  static inline auto
  StartCheck(CheckHandle* handle, uv_check_cb cb) -> Status {
    return {uv_check_start(handle, cb)};
  }

  static inline auto
  StopCheck(CheckHandle* handle) -> Status {
    return {uv_check_stop(handle)};
  }

  class Check : public HandleBaseTemplate<uv_check_t> {
    DEFINE_NON_COPYABLE_TYPE(Check);
  public:
    using Callback = std::function<void (Check*)>;
  private:
    Callback callback_;

    static void OnCheck(CheckHandle* handle);

    auto GetCallback() const -> const Callback& {
      return callback_;
    }
  public:
    explicit Check(Loop* loop,
                   Callback callback,
                   const bool start = true);
    ~Check() override;

    void Start();
    void Stop();
  };
}

#endif //PRT_UV_HANDLE_CHECK_H