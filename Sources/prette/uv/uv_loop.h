#ifndef PRT_UV_LOOP_H
#define PRT_UV_LOOP_H

#include <uv.h>
#include <units.h>
#include <glog/logging.h>
#include "prette/uv/uv_status.h"

namespace prt::uv {
  static inline Status
  InitLoop(uv_loop_t* loop) {
    return Status(uv_loop_init(loop));
  }

  enum RunMode {
    kRunDefault = UV_RUN_DEFAULT,
    kRunNoWait = UV_RUN_NOWAIT,
    kRunOnce = UV_RUN_ONCE,
  };

  static inline std::ostream&
  operator<<(std::ostream& stream, const RunMode& rhs) {
    switch(rhs) {
      case kRunNoWait:
        return stream << "NoWait";
      case kRunOnce:
        return stream << "Once";
      case kRunDefault:
      default:
        return stream << "Default";
    }
  }

  static inline Status
  Run(uv_loop_t* loop, const RunMode mode = kRunDefault) {
    return Status(uv_run(loop, static_cast<uv_run_mode>(mode)));
  }

  static inline void
  StopLoop(uv_loop_t* handle) {
    return uv_stop(handle);
  }

  class Loop {
  protected:
    uv_loop_t loop_;
  public:
    explicit Loop():
      loop_() {
      const auto status = InitLoop(&loop_);
      LOG_IF(FATAL, !status) << "failed to initialize uv_loop_t: " << status;
    }
    virtual ~Loop() {
      //TODO: need to check if closed
    }

    uv_loop_t* GetLoop() {
      return &loop_;
    }

    void Run(const RunMode mode) {
      const auto result = uv::Run(GetLoop(), mode);
      LOG_IF(ERROR, !result) << "failed to run loop: " << result;
    }

    void Stop() {
      StopLoop(GetLoop());
    }

    inline void RunAndWait() {
      return Run(kRunDefault);
    }

    inline void RunOnce() {
      return Run(kRunOnce);
    }

    inline void RunNoWait() {
      return Run(kRunNoWait);
    }

    operator uv_loop_t* () {
      return GetLoop();
    }
  };
}

#endif //PRT_UV_LOOP_H