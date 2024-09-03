#ifndef PRT_ON_SHUTDOWN_H
#define PRT_ON_SHUTDOWN_H

#include "prette/uv/utils.h"

namespace prt {
  class ShutdownListenerHandle;
  class ShutdownListener {
    friend class ShutdownListenerHandle;
  protected:
    ShutdownListener() = default;
    virtual void OnShutdown() = 0;
  public:
    virtual ~ShutdownListener() = default;
  };

  class ShutdownListenerHandle : public uv::AsyncHandleBase {
  private:
    static inline void
    OnCall(uv_async_t* handle) {
      const auto listener = uv::GetHandleData<ShutdownListener>(handle);
      listener->OnShutdown();
    }
  public:
    ShutdownListenerHandle(uv_loop_t* loop, const ShutdownListener* listener):
      uv::AsyncHandleBase(loop, &OnCall) {
      uv::SetHandleData(&handle_, listener);
    }
    ~ShutdownListenerHandle() override = default;
  };
}

#endif //PRT_ON_SHUTDOWN_H