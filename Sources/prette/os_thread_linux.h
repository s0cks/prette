#ifndef PRT_OS_THREAD_LINUX_H
#define PRT_OS_THREAD_LINUX_H

#ifndef PRT_OS_THREAD_H
#error "Please #include <prette/os_thread.h> instead of <prette/os_thread_linux.h>"
#endif //PRT_OS_THREAD_H

#include <pthread.h>

namespace prt {
  static const int kThreadNameMaxLength = 16;
  static const int kThreadMaxResultLength = 128;

  typedef pthread_key_t ThreadLocalKey;
  typedef pthread_t ThreadId;
  typedef void (*ThreadHandler)(void* data);
}

#endif //PRT_OS_THREAD_LINUX_H