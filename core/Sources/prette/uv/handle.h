#ifndef PRT_HANDLE_H
#define PRT_HANDLE_H

#include <cstdint>
#include <functional>
#include <uv.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/uv/status.h"

namespace prt::uv {
template <typename T>
concept HandleType = requires(T value) {
  { value.loop };
  { value.type };
  { value.data };
};

#define CHECK_UV_RESULT(Severity, Result, Message) \
  LOG_IF(Severity, (Result) != UV_OK) << (Message) << ": " << uv_strerror((Result));

#define CHECK_UV(Severity, Result, Message)                   \
  ({                                                          \
    const auto status = (Result);                             \
    LOG_IF(Severity, !status) << (Message) << ": " << status; \
  })

static inline auto Now() -> uint64_t {
  return uv_hrtime();
}

static inline void Close(uv_handle_t* handle, uv_close_cb on_close = nullptr) {
  uv_close(handle, on_close);
}

template <HandleType H>
static inline void Close(H* handle, uv_close_cb on_close = nullptr) {
  return Close((uv_handle_t*)handle, on_close);
}

template <typename D>
static inline void SetHandleData(uv_handle_t* handle, D* data) {
  ASSERT(handle);
  ASSERT(data);
  return uv_handle_set_data(handle, data);
}

template <HandleType H, typename D>
static inline void SetHandleData(H* handle, D* data) {
  ASSERT(handle);
  ASSERT(data);
  return SetHandleData<D>((uv_handle_t*)handle, data);
}

template <typename D>
static inline auto GetHandleData(uv_handle_t* handle) -> D* {
  ASSERT(handle);
  return (D*)uv_handle_get_data(handle);
}

template <HandleType H, typename D>
static inline auto GetHandleData(H* handle) -> D* {
  ASSERT(handle);
  return GetHandleData<D>((uv_handle_t*)handle);
}

class HandleBase {
  DEFINE_NON_COPYABLE_TYPE(HandleBase);

 protected:
  HandleBase() = default;

 public:
  virtual ~HandleBase() = default;
};

template <HandleType H>
class HandleTemplate : public HandleBase {
  DEFINE_NON_COPYABLE_TYPE(HandleTemplate<H>);

 public:
  using Handle = H;

 private:
  H handle_{};
  std::function<void()> on_close_{};

  static void OnClose(uv_handle_t* handle) {
    const auto& on_close = uv::GetHandleData<HandleTemplate<H>>(handle)->on_close_;
    if (on_close)
      on_close();
  }

 protected:
  HandleTemplate() = default;

  inline auto handle_ptr() -> H* {
    return &handle_;
  }

  template <typename D>
  static inline auto Unwrap(H* handle) -> D* {
    return uv::GetHandleData<H, D>(handle);
  }

 public:
  ~HandleTemplate() override = default;

  auto handle() const -> const H& {
    return handle_;
  }

  void Close(std::function<void()> on_close = nullptr) {
    on_close_ = on_close;
    uv_close((uv_handle_t*)HandleTemplate<H>::handle_ptr(), &OnClose);
  }
};

template <HandleType H, typename... Args>
class CallbackHandleTemplate : public HandleTemplate<H> {
 public:
  using Callback = std::function<void(Args...)>;

 private:
  Callback callback_;

 protected:
  explicit CallbackHandleTemplate(const Callback callback) :
    HandleTemplate<H>(),
    callback_(callback) {}

  void Invoke(Args... args) {
    return callback_(args...);
  }

 public:
  ~CallbackHandleTemplate() override = default;
};

using FileHandle = uv_file;
using Buffer = uv_buf_t;
}  // namespace prt::uv

#endif  // PRT_HANDLE_H
