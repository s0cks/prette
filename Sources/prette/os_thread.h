#ifndef PRT_OS_THREAD_H
#define PRT_OS_THREAD_H

#include <string>
#include "prette/rx.h"
#include "prette/common.h"
#include "prette/platform.h"

#ifdef OS_IS_LINUX
#include "prette/os_thread_linux.h"
#elif OS_IS_OSX
#include "prette/os_thread_osx.h"
#elif OS_IS_WINDOWS
#include "prette/os_thread_windows.h"
#endif

namespace prt {
  auto GetCurrentThreadId() -> ThreadId;
  auto GetThreadName(const ThreadId& thread) -> std::string;
  auto SetThreadName(const ThreadId& thread, const std::string& name) -> bool;
  auto InitializeThreadLocal(ThreadLocalKey& key) -> bool;
  auto SetCurrentThreadLocal(const ThreadLocalKey& key, const void* value) -> bool;
  auto GetCurrentThreadLocal(const ThreadLocalKey& key) -> void*;
  auto Start(ThreadId* thread, const std::string& name, const ThreadHandler& func, void* data) -> bool;
  auto Join(const ThreadId& thread) -> bool;
  auto Compare(const ThreadId& lhs, const ThreadId& rhs) -> bool;
  auto GetCurrentThreadCount() -> int;
  auto GetCurrentThreadNames() -> rx::observable<std::string>;

  static inline auto
  GetCurrentThreadName() -> std::string{
    return GetThreadName(GetCurrentThreadId());
  }

  static inline auto
  SetCurrentThreadName(const std::string& name) -> bool{
    return SetThreadName(GetCurrentThreadId(), name);
  }

  class MainThread {
    DEFINE_NON_INSTANTIABLE_TYPE(MainThread);
  private:
    static void SetThreadId(const ThreadId id);
  public:
    static void Init(const ThreadId id = GetCurrentThreadId());
    static auto GetThreadId() -> ThreadId;

    static inline auto
    IsThread(const ThreadId id) -> bool {
      return Compare(GetThreadId(), id) == 0;
    }

    static inline auto
    IsCurrentThread() -> bool {
      return IsThread(GetCurrentThreadId());
    }
  };
}

#endif //PRT_OS_THREAD_H