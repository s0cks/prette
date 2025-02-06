#include "prette/uv/utils.h"

#include <uv.h>

namespace prt::uv {
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
}  // namespace prt::uv