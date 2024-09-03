#ifndef PRT_UV_HANDLE_H
#define PRT_UV_HANDLE_H

#include <uv.h>

namespace prt::uv {
  template<typename H, typename D>
  static inline void
  SetHandleData(H* handle, const D* data) {
    return uv_handle_set_data((uv_handle_t*) handle, (void*) data);
  }

  template<typename H, typename D>
  static inline void
  SetHandleData(H& handle, const D* data) {
    return SetHandleData<H>(&handle, data);
  }

  template<typename D, typename H>
  static inline D*
  GetHandleData(const H* handle) {
    return (D*) uv_handle_get_data((uv_handle_t*) handle);
  }

  class HandleBase {
  protected:
    HandleBase() = default;
  public:
    virtual ~HandleBase() = default;
  };

  template<typename H>
  class HandleBaseTemplate : public HandleBase {
  protected:
    H handle_;

    HandleBaseTemplate() = default;

    inline H* handle() {
      return &handle_;
    }
  public:
    ~HandleBaseTemplate() override = default;
  };
}

#include "prette/uv/uv_handle_async.h"
#include "prette/uv/uv_handle_check.h"
#include "prette/uv/uv_handle_idle.h"
#include "prette/uv/uv_handle_prepare.h"

#endif //PRT_UV_HANDLE_H