#include "prette/uv/utils.h"

#include <uv.h>

#include "prette/common.h"
#include "prette/to_string.h"

namespace prt::uv {
auto Loop::Queue(Work* work) -> Status {
  ASSERT(work);
  return work->Submit(this);
}

void AnyWork::OnWork(uv_work_t* handle) {
  const auto work = Unwrap(handle);
  ASSERT(work);
  return work->on_work_(work);
}

void AnyWork::OnWorkFinished(uv_work_t* handle, const int status) {
  const auto work = Unwrap(handle);
  ASSERT(work);
  work->on_finished_(work, status);
  work->SetFinished();
}

auto AnyWork::Submit(Loop* loop) -> Status {
  ASSERT(loop);
  return uv_queue_work(*loop, handle(), OnWork, OnWorkFinished);
}

auto AnyWork::ToString() const -> std::string {
  return ToStringHelper<AnyWork>{};
}

Loop::Loop() :
  handle_() {
  CHECK_UV(FATAL, InitLoop(&handle_), "failed to initialize uv::Loop");
}

void Loop::Run(const RunMode mode) {
  CHECK_UV(ERROR, RunLoop(GetHandle(), mode), "failed to run uv::Loop");
}

void Loop::Stop() {
  StopLoop(GetHandle());
}

#define DEFINE_UV_HANDLE_INIT(Name, Prefix)               \
  auto Name::Init(Loop* loop, Handle* handle) -> Status { \
    ASSERT(loop);                                         \
    ASSERT(handle);                                       \
    return Prefix##_init(loop->GetHandle(), handle);      \
  }

#define DEFINE_UV_HANDLE_START(Name, Prefix)                      \
  auto Name::Start(Handle* handle, Callback callback) -> Status { \
    ASSERT(handle);                                               \
    ASSERT(callback);                                             \
    return Prefix##_start(handle, callback);                      \
  }

#define DEFINE_UV_HANDLE_STOP(Name, Prefix)   \
  auto Name::Stop(Handle* handle) -> Status { \
    ASSERT(handle);                           \
    return Prefix##_stop(handle);             \
  }

#define DEFINE_UV_HANDLE_CLOSE(Name, Prefix)   \
  auto Name::Close(Handle* handle) -> Status { \
    ASSERT(handle);                            \
    return uv_close((uv_handle_t*)handle);     \
  }

#define DEFINE_UV_HANDLE_FUNCS(Name, Prefix) \
  DEFINE_UV_HANDLE_INIT(Name, Prefix);       \
  DEFINE_UV_HANDLE_START(Name, Prefix);      \
  DEFINE_UV_HANDLE_STOP(Name, Prefix);

DEFINE_UV_HANDLE_FUNCS(Idle, uv_idle);
DEFINE_UV_HANDLE_FUNCS(Prepare, uv_prepare);
DEFINE_UV_HANDLE_FUNCS(Check, uv_check);

#undef DEFINE_UV_HANDLE_FUNCS
#undef DEFINE_UV_HANDLE_STOP
#undef DEFINE_UV_HANDLE_START
#undef DEFINE_UV_HANDLE_INIT

auto Async::Init(Loop& loop, Handle* handle, uv_async_cb on_send) -> Status {
  return uv_async_init(loop, handle, on_send);
}

Async::Async(Loop& loop, uv_async_cb on_send, void* data) {
  CHECK_UV(FATAL, Init(loop, handle(), on_send), "failed to initialize uv_async_t");
  if (data)
    uv::SetHandleData(handle(), data);
}

void Async::Send() {
  CHECK_UV(ERROR, uv::Status(uv_async_send(handle())), "uv_async_send failed");
}

void Async::Close(uv_close_cb on_close) {
  uv::Close(handle(), on_close);
}
}  // namespace prt::uv