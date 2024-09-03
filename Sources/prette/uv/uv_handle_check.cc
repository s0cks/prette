#include "prette/uv/uv_handle.h"

namespace prt::uv {
  void Check::OnCheck(CheckHandle* handle) {
    const auto check = GetHandleData<Check>(handle);
    PRT_ASSERT(check);
    check->GetCallback()(check);
  }

  Check::Check(Loop* loop,
               const Callback& callback,
               const bool start):
    HandleBaseTemplate<CheckHandle>(),
    callback_(callback) {
    SetHandleData(handle(), this);
    const auto status = InitCheck(loop, handle());
    LOG_IF(ERROR, !status) << "failed to initialize uv::Check: " << status;
    if(status && start)
      Start();
  }

  Check::~Check() {
    Stop();
  }

  void Check::Start() {
    const auto status = StartCheck(handle(), &OnCheck);
    LOG_IF(ERROR, !status) << "failed to start uv::Check: " << status;
  }

  void Check::Stop() {
    const auto status = StopCheck(handle());
    LOG_IF(ERROR, !status) << "failed to stop uv::Check: " << status;
  }
}