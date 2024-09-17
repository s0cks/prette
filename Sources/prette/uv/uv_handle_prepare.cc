#include "prette/uv/uv_handle.h"

namespace prt::uv {
  void Prepare::OnPrepare(PrepareHandle* handle) {
    const auto prepare = GetHandleData<Prepare>(handle);
    PRT_ASSERT(prepare);
    prepare->GetCallback()(prepare);
  }

  Prepare::Prepare(Loop* loop,
                   Callback callback,
                   const bool start):
    HandleBaseTemplate<PrepareHandle>(),
    callback_(std::move(callback)) {
    SetHandleData(handle_, this);
    const auto status = InitPrepare(loop, &handle_);
    LOG_IF(ERROR, !status) << "failed to initialize uv::Prepare: " << status;
    if(status && start)
      Start();
  }

  Prepare::~Prepare() {
    Stop();
  }

  void Prepare::Start() {
    const auto status = StartPrepare(handle(), &OnPrepare);
    LOG_IF(ERROR, !status) << "failed to start uv::Prepare: " << status;
  }

  void Prepare::Stop() {
    const auto status = StopPrepare(handle());
    LOG_IF(ERROR, !status) << "failed to stop uv::Prepare: " << status;
  }
}