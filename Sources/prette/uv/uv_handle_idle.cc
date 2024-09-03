#include "prette/uv/uv_handle.h"

namespace prt::uv {
  void Idle::OnIdle(IdleHandle* handle) {
    const auto idle = GetHandleData<Idle>(handle);
    PRT_ASSERT(idle);
    idle->GetCallback()(idle);
  }

  Idle::Idle(Loop* loop,
             const Callback& callback,
             const bool start):
    HandleBaseTemplate<IdleHandle>(),
    callback_(callback) {
    SetHandleData(handle_, this);
    const auto status = InitIdle(loop, &handle_);
    LOG_IF(ERROR, !status) << "failed to initialize uv::Idle: " << status;
    if(status && start)
      Start();
  }

  Idle::~Idle() {
    Stop();
  }

  void Idle::Start() {
    const auto status = StartIdle(&handle_, &OnIdle);
    LOG_IF(ERROR, !status) << "failed to start uv::Idle: " << status;
  }

  void Idle::Stop() {
    const auto status = StopIdle(&handle_);
    LOG_IF(ERROR, !status) << "failed to stop uv::Idle: " << status;
  }
}