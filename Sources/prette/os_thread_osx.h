#ifndef PRT_OS_THREAD_OSX_H
#define PRT_OS_THREAD_OSX_H

#ifndef PRT_OS_THREAD_H
#error "Please #include <prette/os_thread.h> instead of <prette/os_thread_osx.h>"
#endif  // PRT_OS_THREAD_H

#include <pthread.h>  // IWYU pragma: export

namespace prt {
static const int kThreadNameMaxLength = 16;
static const int kThreadMaxResultLength = 128;

using ThreadLocalKey = pthread_key_t;
using ThreadId = pthread_t;
using ThreadHandler = void (*)(void*);
}  // namespace prt

#endif  // PRT_OS_THREAD_LINUX_H