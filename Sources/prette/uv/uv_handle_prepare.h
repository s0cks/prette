#ifndef PRT_UV_HANDLE_H
#error "Please #include <prette/uv/handle.h> instead."
#endif //PRT_UV_HANDLE_H

#ifndef PRT_UV_HANDLE_PREPARE_H
#define PRT_UV_HANDLE_PREPARE_H

#include <functional>
#include "prette/uv/uv_handle.h"
#include "prette/uv/uv_status.h"

namespace prt::uv {
  using PrepareHandle = uv_prepare_t;

  static inline auto
  InitPrepare(uv_loop_t* loop, PrepareHandle* handle) -> Status {
    return {uv_prepare_init(loop, handle)};
  }

  static inline auto
  InitPrepare(Loop* loop, PrepareHandle* handle) -> Status {
    return InitPrepare(loop->GetLoop(), handle);
  }

  static inline auto
  StartPrepare(PrepareHandle* handle, uv_prepare_cb cb) -> Status {
    return {uv_prepare_start(handle, cb)};
  }

  static inline auto
  StopPrepare(PrepareHandle* handle) -> Status {
    return {uv_prepare_stop(handle)};
  }

  class Prepare : public HandleBaseTemplate<PrepareHandle> {
  public:
    using Callback = std::function<void (Prepare *)>;
  private:
    Callback callback_;

    static void OnPrepare(PrepareHandle* handle);

    auto GetCallback() const -> const Callback& {
      return callback_;
    }
  public:
    explicit Prepare(Loop* loop,
                     Callback callback,
                     const bool start = true);
    ~Prepare() override;

    void Start();
    void Stop();
  };
}

#endif //PRT_UV_HANDLE_PREPARE_H