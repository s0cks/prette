#ifndef PRT_UV_UTILS_H
#define PRT_UV_UTILS_H

#include <uv.h>
#include <functional>
#include <glog/logging.h>

#include "prette/common.h"
#include "prette/uv/uv_status.h"

namespace prt::uv {
#define CHECK_UV_RESULT(Severity, Result, Message) \
  LOG_IF(Severity, (Result) != UV_OK) << (Message) << ": " << uv_strerror((Result));

  static inline uint64_t
  Now() {
    return uv_hrtime();
  }

  static inline void
  Close(uv_handle_t* handle, uv_close_cb cb = NULL) {
    uv_close(handle, cb);
  }

  template<typename T>
  static inline void
  Close(T* handle, uv_close_cb cb = NULL) {
    return Close((uv_handle_t*)handle, cb);
  }
}

#endif //PRT_UV_UTILS_H